import qbs.base 1.0
import "../../test.qbs" as Test

Test {
    name: "tst_dir"
    files: [ "*.cpp", "*.h", "*.qrc" ]
}
