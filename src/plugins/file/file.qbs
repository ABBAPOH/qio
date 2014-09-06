import qbs.base 1.0
import "../plugin.qbs" as Plugin

Plugin {
    Depends { name: "Qt.concurrent" }
    name: "file"

    files: [
        "fileengine.cpp",
        "fileengine.h",
        "fileenginefallback.cpp",
        "fileenginefallback.h",
        "fileengineplugin.cpp",
        "fileengineplugin.h"
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
