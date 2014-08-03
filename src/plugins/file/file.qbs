import qbs.base 1.0
import "../plugin.qbs" as Plugin

Plugin {
    Depends { name: "Qt.concurrent" }
    name: "file"

}
