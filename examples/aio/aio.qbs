import qbs.base 1.0

Product {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    type: "application"
    destinationDirectory: project.install_binary_path

    cpp.dynamicLibraries: [ "rt" ]

    files: [ "*.cpp", "*.h" ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_binary_path
    }
}
