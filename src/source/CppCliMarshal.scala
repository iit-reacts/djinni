package djinni

import djinni.ast.Record.DerivingType
import djinni.ast.Record.DerivingType.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._

class CppCliMarshal(spec: Spec) extends Marshal(spec) {


  override def typename(tm: meta.MExpr): String = toCppCliType(tm, None, Seq(), true)
  def typename(ty: TypeRef, needsHandle: Boolean): String = toCppCliType(ty, None, Seq(), needsHandle)
  def typename(name: String, ty: TypeDef) = idCs.ty(name)

  override def fqTypename(tm: meta.MExpr): String = throw new AssertionError("not applicable")

  override def paramType(tm: meta.MExpr): String = typename(tm)
  def paramType(tm: MExpr, scopeSymbols: Seq[String]): String = toCppCliType(tm, None, scopeSymbols, true)
  def paramType(ty: TypeRef, scopeSymbols: Seq[String]): String = paramType(ty.resolved, scopeSymbols)

  override def fqParamType(tm: meta.MExpr): String = throw new AssertionError("not applicable")

  override def returnType(ret: Option[TypeRef]): String = ret.fold("void")(toCppCliType(_))

  override def fqReturnType(ret: Option[TypeRef]): String = throw new AssertionError("not applicable")

  override def fieldType(tm: meta.MExpr): String = typename(tm)

  override def fqFieldType(tm: meta.MExpr): String = throw new AssertionError("not applicable")

  private def using(fqName: String) = {
    DeclRef(s"using $fqName;", Some(spec.cppCliNamespace))
  }

  def include(ident: String, isExtendedRecord: Boolean = false): String = {
    q(spec.csIdentStyle.ty(ident) + "." + spec.cppHeaderExt)
  }

  def references(m: Meta): Seq[SymbolReference] = m match {
    case o: MOpaque =>
      o match {
        case MString => List(using("System::String"))
        case MBinary => List(using("System::Byte"))
        case MList => List(using("System::Collections::Generic::List"))
        case MSet => List(using("System::Collections::Generic::HashSet"))
        case MMap => List(using("System::Collections::Generic::Dictionary"))
        case MDate => List(using("System::DateTime"))
        case _ => List()
      }
    case d: MDef =>
      List(ImportRef(include(d.name)))
    case _ => List()
  }

  def references(d: DerivingType): Seq[SymbolReference] = d match {
    case DerivingType.Eq =>
      List(
        using("System::IEquatable"),
        using("System::Object"),
        using("System::String")
      )
    case DerivingType.Ord => List(using("System::IComparable"))
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
            case _: MPrimitive => "System::Nullable" + args + handle(tm.base)
            case _ => expr(arg)
          }
        case e: MExtern => throw new AssertionError("unreachable")
        case o =>
          val base = o match {
            case p: MPrimitive => p.cppCliName
            case MString => "String"
            case MDate => "DateTime"
            case MBinary => "array<Byte>"
            case MOptional => throw new AssertionError("optional should have been special cased")
            case MList => "List"
            case MSet => "HashSet"
            case MMap => "Dictionary"
            case d: MDef => withNamespace(idCs.ty(d.name))
            case e: MExtern => throw new AssertionError("unreachable")
            case p: MParam => idCs.typeParam(p.name)
          }
          base + args + handle(o)
      }
    }
    expr(tm)
  }
}
