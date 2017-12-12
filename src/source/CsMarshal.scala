package djinni

import djinni.ast._
import djinni.generatorTools._
import djinni.meta._

class CsMarshal(spec: Spec) extends Marshal(spec) {

  override def typename(tm: MExpr): String = toCsType(tm)
  def typename(name: String, ty: TypeDef) = idCs.ty(name)

  override def fqTypename(tm: MExpr): String = throw new AssertionError("not applicable")

  override def paramType(tm: MExpr): String = typename(tm)

  override def fqParamType(tm: MExpr): String = throw new AssertionError("not applicable")

  override def returnType(ret: Option[TypeRef]): String = ret.fold("void")(toCsType(_))

  override def fqReturnType(ret: Option[TypeRef]): String = throw new AssertionError("not applicable")

  override def fieldType(tm: MExpr): String = typename(tm)

  override def fqFieldType(tm: MExpr): String = throw new AssertionError("not applicable")

  def references(m: Meta): Seq[SymbolReference] = m match {
    case o: MOpaque =>
      o match {
        case MList => List(ImportRef("System.Collections.Generic"))
        case MSet => List(ImportRef("System.Collections.Generic"))
        case MMap => List(ImportRef("System.Collections.Generic"))
        case MDate => List(ImportRef("System"))
        case _ => List()
      }
    case _ => List()
  }

  def toCsType(ty: TypeRef): String = toCsType(ty.resolved)

  def toCsType(tm: MExpr): String = {
    def args(tm: MExpr) = if (tm.args.isEmpty) "" else tm.args.map(f(_, true)).mkString("<", ", ", ">")
    def f(tm: MExpr, needRef: Boolean): String = {
      tm.base match {
        case MOptional =>
          // HACK: We use "null" for the empty optional in Java.
          assert(tm.args.size == 1)
          val arg = tm.args.head
          arg.base match {
            case p: MPrimitive => p.csName + "?"
            case MOptional => throw new AssertionError("nested optional?")
            case m => f(arg, true)
          }
        case e: MExtern => throw new AssertionError("unreachable")
        case o =>
          val base = o match {
            case p: MPrimitive => p.csName
            case MString => "string"
            case MDate => "DateTime"
            case MBinary => "byte[]"
            case MOptional => throw new AssertionError("optional should have been special cased")
            case MList => "List"
            case MSet => "HashSet"
            case MMap => "Dictionary"
            case d: MDef => idCs.ty(d.name)
            case e: MExtern => throw new AssertionError("unreachable")
            case p: MParam => idCs.typeParam(p.name)
          }
          base + args(tm)
      }
    }
    f(tm, false)
  }
}
