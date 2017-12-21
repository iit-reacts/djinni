package djinni

import djinni.ast.Record.DerivingType
import djinni.ast.Record.DerivingType.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._

class CppCliMarshal(spec: Spec) extends Marshal(spec) {
  private val cppMarshal = new CppMarshal(spec)

  override def typename(tm: meta.MExpr): String = toCppCliType(tm, None, Seq(), true)
  def typename(ty: TypeRef, needsHandle: Boolean): String = toCppCliType(ty, None, Seq(), needsHandle)
  def typename(name: String, ty: TypeDef) = idCs.ty(name)

  override def fqTypename(tm: meta.MExpr): String = typename(tm)

  override def paramType(tm: meta.MExpr): String = typename(tm)
  def paramType(tm: MExpr, scopeSymbols: Seq[String]): String = toCppCliType(tm, None, scopeSymbols, true)
  def paramType(ty: TypeRef, scopeSymbols: Seq[String]): String = paramType(ty.resolved, scopeSymbols)

  override def fqParamType(tm: meta.MExpr): String = throw new AssertionError("not applicable")

  override def returnType(ret: Option[TypeRef]): String = ret.fold("void")(toCppCliType(_))

  override def fqReturnType(ret: Option[TypeRef]): String = throw new AssertionError("not applicable")

  override def fieldType(tm: meta.MExpr): String = typename(tm)

  override def fqFieldType(tm: meta.MExpr): String = throw new AssertionError("not applicable")

  def helperClass(name: String) = idCpp.ty(name)
  private def helperClass(tm: MExpr): String = helperName(tm) + helperTemplates(tm)

  override def toCpp(tm: MExpr, expr: String): String = {
    s"${helperClass(tm)}::ToCpp($expr)"
  }

  override def fromCpp(tm: MExpr, expr: String): String = {
    s"${helperClass(tm)}::FromCpp($expr)"
  }

  private def using(fqName: String) = {
    DeclRef(s"using $fqName;", Some(spec.cppCliNamespace))
  }

  def include(ident: String, isExtendedRecord: Boolean = false): String = {
    q(spec.csIdentStyle.file(ident) + "." + spec.cppHeaderExt)
  }

  def references(m: Meta, exclude: String): Seq[SymbolReference] = m match {
    case d: MDef =>
      if (d.name != exclude) {
        List(ImportRef(include(d.name)))
      } else {
        List()
      }
    case _ => List()
  }

  def references(d: DerivingType): Seq[SymbolReference] = d match {
    case DerivingType.Eq =>
      List(
        using("System::IEquatable"),
        using("System::Object"),
        using("System::String")
      )
    case _ => List()
  }

  private def toCppCliType(ty: TypeRef, namespace: Option[String] = None, scopeSymbols: Seq[String] = Seq(), needsHandle: Boolean = true): String =
    toCppCliType(ty.resolved, namespace, scopeSymbols, needsHandle)

  private def toCppCliType(tm: MExpr, namespace: Option[String], scopeSymbols: Seq[String], needsHandle: Boolean): String = {
    def withNamespace(name: String): String = {
      // If an unqualified symbol needs to have its namespace added, this code assumes that the
      // namespace is the one that's defined for generated types (spec.cppNamespace).
      // This seems like a safe assumption for the C++ generator as it doesn't make much use of
      // other global names, but might need to be refined to cover other types in the future.
      val ns = namespace match {
        case Some(ns) => Some(ns)
        case None => if (scopeSymbols.contains(name)) Some(spec.csNamespace.replace(".", "::")) else None
      }
      withNs(ns, name)
    }
    def handle(o: Meta) = {
      if (needsHandle) {
        o match {
          case _: MPrimitive => ""
          case d: MDef =>
            d.defType match {
              case DEnum => ""
              case _ => "^"
            }
          case MOptional => ""
          case MDate => ""
          case e: MExtern => if (e.cs.reference) "^" else ""
          case _ => "^"
        }
      } else ""
    }
    def expr(tm: MExpr): String = {
      val args = if (tm.args.isEmpty) "" else tm.args.map((tm: MExpr) => expr(tm)).mkString("<", ", ", ">")
      tm.base match {
        case MOptional =>
          assert(tm.args.size == 1)
          val arg = tm.args.head
          arg.base match {
            case _: MPrimitive => "System::Nullable" + args
            case MDate => "System::Nullable" + args
            case d: MDef =>
              d.defType match {
                case DEnum => "System::Nullable" + args
                case _ => expr(arg)
              }
            case e: MExtern =>
              if (e.cs.reference) {
                expr(arg)
              } else {
                "System::Nullable" + args
              }
            case _ => expr(arg)
          }
        case e: MExtern => withNamespace(e.cs.typename)
        case o =>
          val base = o match {
            case p: MPrimitive => p.cppCliName
            case MString => "System::String"
            case MDate => "System::DateTime"
            case MBinary => "array<System::Byte>"
            case MList => "System::Collections::Generic::List"
            case MSet => "System::Collections::Generic::HashSet"
            case MMap => "System::Collections::Generic::Dictionary"
            case d: MDef => withNamespace(idCs.ty(d.name))
            case p: MParam => idCs.typeParam(p.name)
            case MOptional => throw new AssertionError("optional should have been special cased")
            case _: MExtern => throw new AssertionError("optional should have been special cased")
          }
          base + args + handle(o)
      }
    }
    expr(tm)
  }

  def helperName(tm: MExpr): String = tm.base match {
    case d: MDef => d.defType match {
      case DEnum => withNs(Some("djinni"), s"Enum<${cppMarshal.fqTypename(tm)}, ${fqTypename(tm)}>")
      case _ => withNs(Some(spec.csNamespace.replace(".", "::")), helperClass(d.name))
    }
    case e: MExtern => e.objcpp.translator
    case o => withNs(Some("djinni"), o match {
      case p: MPrimitive => p.idlName match {
        case "i8" => "I8"
        case "i16" => "I16"
        case "i32" => "I32"
        case "i64" => "I64"
        case "f32" => "F32"
        case "f64" => "F64"
        case "bool" => "Bool"
      }
      case MOptional => "Optional"
      case MBinary => "Binary"
      case MDate => "Date"
      case MString => if (spec.cppUseWideStrings) "WString" else "String"
      case MList => "List"
      case MSet => "Set"
      case MMap => "Map"
      case d: MDef => throw new AssertionError("unreachable")
      case e: MExtern => throw new AssertionError("unreachable")
      case p: MParam => throw new AssertionError("not applicable")
    })
  }

  private def helperTemplates(tm: MExpr): String = {
    def f() = if(tm.args.isEmpty) "" else tm.args.map(helperClass).mkString("<", ", ", ">")
    tm.base match {
      case MOptional =>
        assert(tm.args.size == 1)
        val argHelperClass = helperClass(tm.args.head)
        s"<${spec.cppOptionalTemplate}, $argHelperClass>"
      case MList | MSet =>
        assert(tm.args.size == 1)
        f
      case MMap =>
        assert(tm.args.size == 2)
        f
      case _ => f
    }
  }
}
