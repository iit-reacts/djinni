package djinni

import java.io.File

object util {
  def toUnixPath(path: File): String = {
    val absolutePath = path.getAbsolutePath
    if (File.separatorChar != '/')
      absolutePath.replace(File.separatorChar, '/')
    else
      absolutePath
  }
}
