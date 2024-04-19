param([Parameter(Mandatory)][string]$ARCH)

echo "Building libraries..."

cd ../../../

.\devShell.ps1 -ARCH "$ARCH"
.\build_windows.ps1 -ARCH "$ARCH"

cd test/benchmark/smart/

echo "Compiling Tests..."

.\build.ps1 -ARCH "$ARCH" -TEST string_append -Config "Release"
.\build.ps1 -ARCH "$ARCH" -TEST string_append_cont -Config "Release"

.\build.ps1 -ARCH "$ARCH" -TEST prepend -Config "Release"
.\build.ps1 -ARCH "$ARCH" -TEST prepend_cont -Config "Release"

.\build.ps1 -ARCH "$ARCH" -TEST insert_middle -Config "Release"
.\build.ps1 -ARCH "$ARCH" -TEST insert_middle_cont -Config "Release"

.\build.ps1 -ARCH "$ARCH" -TEST construct -Config "Release"

.\build.ps1 -ARCH "$ARCH" -TEST iterate -Config "Release"
.\build.ps1 -ARCH "$ARCH" -TEST iterate_fragmentation -Config "Release"

echo "Running Tests..."

.\autotest_win.ps1 -TEST string_append -OPT 1
.\autotest_win.ps1 -TEST string_append_cont -OPT 1

.\autotest_win.ps1 -TEST prepend -OPT 1
.\autotest_win.ps1 -TEST prepend_cont -OPT 1

.\autotest_win.ps1 -TEST insert_middle -OPT 1
.\autotest_win.ps1 -TEST insert_middle_cont -OPT 1

.\autotest_win.ps1 -TEST construct -OPT 1

.\autotest_win.ps1 -TEST iterate -OPT 1
.\autotest_win.ps1 -TEST iterate_fragmentation -OPT 1

echo "Finished!"
