import qbs.base 1.0
import "../plugin.qbs" as Plugin

Plugin {
    Depends { name: "Qt.concurrent" }
    name: "file"

    files: [
        "fileenginefallback.cpp",
        "fileenginefallback.h",
        "fileplugin.cpp",
        "fileplugin.h",
        "syncdirengine.h",
        "syncdirengine.cpp"
    ]

    Group {
        name: "windows"
        condition: qbs.targetOS.contains("windows")
        files: [
            "fileenginewin.cpp",
            "fileenginewin.h",
        ]
    }
}
