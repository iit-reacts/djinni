package djinni

import djinni.ast._
import djinni.generatorTools._
import djinni.writer.IndentWriter

import scala.collection.mutable

class CppCliGenerator(spec: Spec) extends Generator(spec) {

  class CppCliRefs() {
    val cppCli = new mutable.TreeSet[String]()
  }

  def writeCppCliHppFile(name: String, origin: String, includes: Iterable[String], fwds: Iterable[String], f: IndentWriter => Unit, f2: IndentWriter => Unit = (w => {})) =
    writeHppFileGeneric(spec.cppCliOutFolder.get, spec.csNamespace.replace(".", "::"), spec.csIdentStyle.ty)(name, origin, includes, fwds, f, f2)

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
    val refs = new CppCliRefs()

    writeCppCliHppFile(ident, origin, refs.cppCli, Nil, (w: IndentWriter) => {
      w.wl(s"test!")
    })
    // TODO
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    // TODO
  }

  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {
    // TODO
  }
}
