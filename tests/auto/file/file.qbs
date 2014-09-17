import qbs.base 1.0
import "../../test.qbs" as Test

Test {
    type: "application"
    name: "tst_file"
    destinationDirectory: project.install_binary_path

    files: [
        "*.cpp",
        "*.h",
        "*.qrc"
    ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_binary_path
    }
}
