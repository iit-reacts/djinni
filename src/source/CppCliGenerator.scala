package djinni

import djinni.ast.Record.DerivingType
import djinni.ast.Record.DerivingType.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._
import djinni.writer.IndentWriter

import scala.collection.mutable

class CppCliGenerator(spec: Spec) extends Generator(spec) {

  val marshal = new CppCliMarshal(spec)

  class CppCliRefs() {
    val hpp = new mutable.TreeSet[String]()
    val hppFwds = new mutable.TreeSet[String]()

    def find(d: DerivingType) = for(r <- marshal.references(d)) addRefs(r)
    def find(ty: TypeRef) { find(ty.resolved) }
    def find(tm: MExpr) {
      tm.args.foreach(find)
      find(tm.base)
    }
    def find(m: Meta) = for(r <- marshal.references(m)) addRefs(r)

    private def addRefs(r: SymbolReference) = r match {
      case ImportRef(arg) => hpp.add("#include " + arg)
      case DeclRef(decl, Some(spec.cppCliNamespace)) => hppFwds.add(decl)
      case DeclRef(_, _) =>
    }
  }

  def writeCppCliHppFile(name: String, origin: String, includes: Iterable[String], fwds: Iterable[String], f: IndentWriter => Unit, f2: IndentWriter => Unit = (w => {})) =
    writeHppFileGeneric(spec.cppCliOutFolder.get, spec.csNamespace.replace(".", "::"), spec.csIdentStyle.ty)(name, origin, includes, fwds, f, f2)

  def generateConstants(w: IndentWriter, ident: Ident, r: Record, consts: Seq[Const]): Unit = {
    def writeCppCliLiteral(ty: TypeRef, v: Any) = {
      v match {
        case l: Long if List("long", "long?").contains(marshal.fieldType(ty)) => w.w(l.toString + "L")
        case l: Long => w.w(l.toString)
        case d: Double if marshal.fieldType(ty).equals("float") => w.w(d.toString + "F")
        case d: Double if marshal.fieldType(ty).equals("float?") => w.w(d.toString + "F")
        case d: Double => w.w(d.toString)
        case b: Boolean => w.w(if (b) "true" else "false")
        case s: String => w.w(s)
        case e: EnumValue => w.w(s"${marshal.typename(ty)}.${idCs.enum(e)}")
        case v: ConstRef => w.w(idCs.const(v))
        case z: Map[_, _] => { // Value is record
          val recordMdef = ty.resolved.base.asInstanceOf[MDef]
          val record = recordMdef.body.asInstanceOf[Record]
          val vMap = z.asInstanceOf[Map[String, Any]]
          w.wl(s"gcnew ${marshal.typename(ty, false)}(")
          w.increase()
          // Use exact sequence
          val skipFirst = SkipFirst()
          for (f <- record.fields) {
            skipFirst {
              w.wl(",")
            }
            writeCppCliConst(w, f.ty, vMap.apply(f.ident.name))
            w.w(" /* " + idCs.field(f.ident) + " */ ")
          }
          w.w(")")
          w.decrease()
        }
      }
    }
    def writeCppCliConst(w: IndentWriter, ty: TypeRef, v: Any): Unit = ty.resolved.base match {
      case MOptional =>
        w.w(s"gcnew ${marshal.typename(ty, false)}(")
        writeCppCliLiteral(ty, v)
        w.w(")")
      case _ => writeCppCliLiteral(ty, v)
    }
    def isInitOnly(c: Const) = c.ty.resolved.base match {
      case _: MPrimitive => false
      case _ => true
    }

    for (c <- consts) {
      w.wl
      writeDoc(w, c.doc)
      val fieldType = marshal.fieldType(c.ty)
      val fieldIdent = idCs.const(c.ident)
      if (isInitOnly(c)) {
        w.wl(s"static initonly $fieldType $fieldIdent;")
      } else {
        w.w(s"literal $fieldType $fieldIdent = ")
        writeCppCliConst(w, c.ty, c.value)
        w.wl(";")
      }
    }

    val initOnlyConsts = consts.filter(c => isInitOnly(c))
    if (initOnlyConsts.nonEmpty) {
      w.wl
      val self = marshal.typename(ident, r)
      w.w(s"static $self()").braced {
        initOnlyConsts.foreach(c => {
          w.w(s"${idCs.const(c.ident)} = ")
          writeCppCliConst(w, c.ty, c.value)
          w.wl(";")
        })
      }
    }
  }

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
    // TODO
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    val refs = new CppCliRefs()
    refs.find(MString) // for: String^ ToString();
    r.fields.foreach(f => refs.find(f.ty))
    r.consts.foreach(c => refs.find(c.ty))
    r.derivingTypes.foreach(d => refs.find(d))

    val self = marshal.typename(ident, r)

    writeCppCliHppFile(ident, origin, refs.hpp, refs.hppFwds, w => {
      writeDoc(w, doc)

      val inheritanceList = if (r.derivingTypes.nonEmpty) {
        r.derivingTypes.map {
          case DerivingType.Eq => s"IEquatable<$self^>"
          case DerivingType.Ord => s"IComparable<$self^>"
          case _ => throw new AssertionError("unreachable")
        }.mkString(" : ", ", ", "")
      } else ""

      w.w(s"public ref class $self$inheritanceList").bracedSemi {
        w.wlOutdent("public:")

        generateConstants(w, ident, r, r.consts)

        // Properties.
        for (f <- r.fields) {
          w.wl
          val retType = s"${marshal.fieldType(f.ty)}"
          w.wl(s"property $retType ${idCs.property(f.ident)}").braced {
            w.wl(s"$retType get();")
            // TODO Implementation!
          }
        }

        // Constructor.
        if (r.fields.nonEmpty) {
          w.wl
          writeAlignedCall(w, self + "(", r.fields, ")", f => marshal.fieldType(f.ty) + " " + idCs.local(f.ident))
          w.wl
          val init = (f: Field) => "_" + idCs.field(f.ident) + "(" + idCs.local(f.ident) + ")"
          w.wl(": " + init(r.fields.head))
          r.fields.tail.map(f => ", " + init(f)).foreach(w.wl)
          w.wl("{}")
        }

        w.wl
        w.wl(s"String^ ToString() override").braced {
          w.wl(s"""return "$self[TODO]"; // TODO""")
        }

        def call(f: Field) = {
          f.ty.resolved.base match {
            case p: MPrimitive => "."
            case _ => "->"
          }
        }

        if (r.derivingTypes.contains(DerivingType.Eq)) {
          w.wl
          w.w(s"virtual bool Equals($self^ other)").braced {
            w.wl("if (ReferenceEquals(nullptr, other)) return false;")
            w.wl("if (ReferenceEquals(this, other)) return true;")
            w.wl(r.fields.map(f => {
              val property = idCs.property(f.ident)
              f.ty.resolved.base match {
                case p: MPrimitive => s"$property == other->$property"
                case _ => s"$property->Equals(other->$property)"
              }
            }).mkString("return ", " && ", ";"))
          }

          w.wl
          w.w("bool Equals(Object^ obj) override").braced {
            w.wl("if (ReferenceEquals(nullptr, obj)) return false;")
            w.wl("if (ReferenceEquals(this, obj)) return true;")
            w.wl(s"return obj->GetType() == GetType() && Equals(($self^) obj);")
          }

          w.wl
          w.w("int GetHashCode() override").braced {
            w.wl(s"auto hashCode = ${idCs.property(r.fields.head.ident)}${call(r.fields.head)}GetHashCode();")
            for (f <- r.fields.tail) {
              w.wl(s"hashCode = (hashCode * 397) ^ ${idCs.property(f.ident)}${call(f)}GetHashCode();")
            }
            w.wl("return hashCode;")
          }
        }

        if (r.derivingTypes.contains(DerivingType.Ord)) {
          def compare(f: Field): String = {
            val property = idCs.property(f.ident)
            f.ty.resolved.base match {
              case MString => s"string.Compare($property, other.$property, StringComparison.Ordinal)"
              case _ => s"$property.CompareTo(other.$property)"
            }
          }
          w.wl
          w.w(s"int CompareTo($self other)").braced {
            w.wl("if (ReferenceEquals(this, other)) return 0;")
            w.wl("if (ReferenceEquals(null, other)) return 1;")
            for (f <- r.fields.dropRight(1)) {
              val local = idCs.local(f.ident) + "Comparison"
              w.wl(s"var $local = ${compare(f)};")
              w.wl(s"if ($local != 0) return $local;")
            }
            w.wl(s"return ${compare(r.fields.last)};")
          }
        }

        w.wl
        w.wlOutdent("private:")

        // Field definitions.
        for (f <- r.fields) {
          writeDoc(w, f.doc)
          w.wl(marshal.fieldType(f.ty) + " _" + idCs.field(f.ident) + ";")
        }
      }
    })
  }

  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {
    if (!i.ext.cpp) return

    val refs = new CppCliRefs()
    i.methods.foreach(m => {
      m.params.foreach(p => refs.find(p.ty))
      m.ret.foreach((x) => refs.find(x))
    })
    i.consts.foreach(c => {
      //      refs.find(c.ty)
    })

    val methodNamesInScope = i.methods.map(m => idCs.method(m.ident))

    writeCppCliHppFile(ident, origin, refs.hpp, refs.hppFwds, w => {
      val self = marshal.typename(ident, i)
      w.w(s"public ref class $self").bracedSemi {
        i.methods.foreach(m => {
          w.wl
          val ret = marshal.returnType(m.ret)
          val static = if (m.static) "static " else ""
          val params = m.params.map(p => {
            marshal.paramType(p.ty, methodNamesInScope) + " " + idCs.local(p.ident)
          })
          w.w(static + ret + " " + idCs.method(m.ident) + params.mkString("(", ", ", ")")).braced {

          }
        })
      }
    })
  }
}
