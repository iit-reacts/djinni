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
    def base(tm: Meta): String = tm match {
      case p: MPrimitive => p.csName
      case MString => "string"
      case MDate => "Date"
      case MBinary => "Bin"
      case MOptional => "Nullable"
      case MList => "List"
      case MSet => "Set"
      case MMap => "Map"
      case d: MDef => idCs.ty(d.name)
      case e: MExtern => throw new AssertionError("unreachable")
      case p: MParam => idCs.typeParam(p.name)
    }
    def expr(tm: MExpr): String = {
      if (isOptionalInterface(tm)) {
        expr(tm.args.head)
      } else {
        val args = if (tm.args.isEmpty) "" else tm.args.map(expr).mkString("<", ", ", ">")
        base(tm.base) + args
      }
    }
    expr(tm)
  }
}
