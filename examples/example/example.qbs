import qbs.base 1.0

Application {
    name: "example"
    type: "application"
    destinationDirectory: project.install_app_path

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    Depends { name: "qio" }

    cpp.includePaths: project.includePaths
    cpp.cxxFlags: project.cxxFlags
    cpp.linkerFlags: project.linkerFlags
    cpp.minimumOsxVersion: "10.7"

    files: [
        "*.cpp",
        "*.h"
    ]
    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_app_path
    }
}
