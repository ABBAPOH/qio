import qbs.base 1.0

DynamicLibrary {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "qio" }

    cpp.includePaths: project.includePaths
    cpp.cxxFlags: project.cxxFlags
    cpp.linkerFlags: project.linkerFlags
    cpp.minimumOsxVersion: "10.7"

    files: [
        "*.cpp",
        "*.h",
    ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_plugin_path
    }
}
