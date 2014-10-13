import qbs.base 1.0
import "../../test.qbs" as Test

Test {
    name: "tst_fileentry"
    files: [ "*.cpp", "*.h", "*.qrc" ]
}
