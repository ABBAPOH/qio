import qbs.base 1.0

DynamicLibrary {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.core-private" }

    name: "qio"
    destinationDirectory: project.install_library_path
    cpp.defines: [
        "QIO_EXPORT=Q_DECL_EXPORT",
        "PLUGINS_DIR=\"" + project.install_plugin_path + "\""
    ]
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

    Export {
        Depends { name: "cpp" }
        cpp.defines: [ "QIO_EXPORT=Q_DECL_IMPORT" ]
    }

    Properties {
         condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
         cpp.rpaths: "$ORIGIN"
    }

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
