import qbs.base 1.0

Product {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.test" }
    Depends { name: "qio" }

    type: "application"
    destinationDirectory: project.install_binary_path
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
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        cpp.rpaths: [ "$ORIGIN/../lib" + project.lib_suffix + "/" + project.app_target ]
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_binary_path
    }
}
