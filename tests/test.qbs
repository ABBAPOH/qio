import qbs.base 1.0

DynamicLibrary {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.test" }
    Depends { name: "qio" }

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

    Properties {
        condition: qbs.targetOS.contains("unix")
        cpp.rpaths: [ "$ORIGIN/../lib" + project.lib_suffix ]
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_plugin_path
    }
}
