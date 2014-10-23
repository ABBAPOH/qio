import qbs.base 1.0
import "../../test.qbs" as Test

Test {
    name: "tst_archive"
    condition: project.withArchive
    files: [ "*.cpp", "*.h", "*.qrc" ]
}
