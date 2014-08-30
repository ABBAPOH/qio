import qbs.base 1.0

Project {
    property string app_target: "qio"

    property string install_app_path: "bin"
    property string lib_suffix: ""

    property string install_binary_path: {
        return install_app_path
    }

    property string install_library_path: {
        if (qbs.targetOS.contains("osx"))
            return "lib/" + app_target
        else if (qbs.targetOS.contains("windows"))
            return install_app_path
        else
            return "lib" + lib_suffix + "/" + app_target
    }

    property string install_plugin_path: {
        return install_library_path + "/plugins/filesystems"
    }

    property stringList includePaths: {
        return [ path + "/include" ]
    }

    property stringList libraryPaths: {
        return []
    }

    property stringList cxxFlags: {
        if (qbs.targetOS.contains("osx"))
            return [ "-std=c++0x", "-stdlib=libc++" ]
        if (qbs.targetOS.contains("linux"))
            return [ "-std=c++0x", "-Werror" ]
        else
            return []
    }

    property stringList linkerFlags: {
        if (qbs.targetOS.contains("osx"))
            return [ "-stdlib=libc++" ]
        else
            return []
    }

    name: "QIO"

    SubProject {
        filePath: "examples/examples.qbs"
    }
    SubProject {
        filePath: "src/src.qbs"
    }
}

