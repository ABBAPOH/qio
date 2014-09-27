import qbs.base 1.0
import "../../test.qbs" as Test

Test {
    name: "tst_file"
    files: [ "*.cpp", "*.h", "*.qrc" ]
}
