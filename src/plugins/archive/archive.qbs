import qbs.base 1.0
import "../plugin.qbs" as Plugin

Plugin {
    name: "archive"
    condition: project.withArchive

    Depends { name: "cpp" }
    cpp.dynamicLibraries: [ "KF5Archive" ]

    files: [ "*.cpp",  "*.h" ]
}
