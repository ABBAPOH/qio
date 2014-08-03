import qbs.base 1.0

DynamicLibrary {
    name: "qio"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.core-private" }

    cpp.defines: [ "PLUGINS_DIR=\"" + project.install_plugin_path + "\""]

    files: [
        "*.cpp",
        "*.h",
    ]
    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_library_path
    }
}
