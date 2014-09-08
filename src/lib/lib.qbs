import qbs.base 1.0

    DynamicLibrary {
    name: "qio"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.core-private" }

    cpp.defines: [
        "QIO_EXPORT=Q_DECL_EXPORT",
        "PLUGINS_DIR=\"" + project.install_plugin_path + "\""
    ]

    Export {
        Depends { name: "cpp" }
        cpp.defines: [ "QIO_EXPORT=Q_DECL_IMPORT" ]
    }
    cpp.includePaths: project.includePaths
    cpp.cxxFlags: {
        if (qbs.targetOS.contains("osx"))
            return [ "-std=c++11", "-stdlib=libc++" ]
        if (qbs.targetOS.contains("linux"))
            return [ "-std=c++11", "-Werror" ]
        if (qbs.targetOS.contains("windows") && cpp.compilerName.contains("g++"))
            return "-std=c++11";
        else
            return []
    }
    cpp.linkerFlags: qbs.targetOS.contains("osx") ? [ "-stdlib=libc++" ] : []
    cpp.minimumOsxVersion: "10.7"

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
