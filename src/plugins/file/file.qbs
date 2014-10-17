import qbs.base 1.0
import "../plugin.qbs" as Plugin

Plugin {
    Depends { name: "Qt.concurrent" }
    name: "file"

    files: [
        "fileenginefallback.cpp",
        "fileenginefallback.h",
        "fileentryengine.h",
        "fileentryengine.cpp",
        "fileplugin.cpp",
        "fileplugin.h"
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
