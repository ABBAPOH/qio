import qbs.base 1.0

Application {
    type: "application"
    destinationDirectory: project.install_app_path

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
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

    files: [
        "*.cpp",
        "*.h",
        "mainwindow.ui"
    ]
    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_app_path
    }
}
