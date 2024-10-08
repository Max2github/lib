param([Parameter(Mandatory)][string]$ARCH)

$ErrorActionPreference = "Stop"

echo "Building libraries..."

cd ../../../

.\devShell.ps1 -ARCH "$ARCH"
.\build_windows.ps1 -ARCH "$ARCH"

cd test/benchmark/smart/

echo "Compiling Tests..."

$CXXFLAGS = '-DINCLUDE_C_SOURCES', '-DMEMORY_TEST'

.\build.ps1 -ARCH "$ARCH" -TEST string_append -Config "Release" -CXXFLAGS $CXXFLAGS

.\build.ps1 -ARCH "$ARCH" -TEST string_append_cont -Config "Release" -CXXFLAGS $CXXFLAGS

.\build.ps1 -ARCH "$ARCH" -TEST prepend -Config "Release" -CXXFLAGS $CXXFLAGS
.\build.ps1 -ARCH "$ARCH" -TEST prepend_cont -Config "Release" -CXXFLAGS $CXXFLAGS

.\build.ps1 -ARCH "$ARCH" -TEST insert_middle -Config "Release" -CXXFLAGS $CXXFLAGS
.\build.ps1 -ARCH "$ARCH" -TEST insert_middle_cont -Config "Release" -CXXFLAGS $CXXFLAGS

echo "Running Tests..."

.\autotest_mem.ps1 -TEST string_append -OPT 1
.\autotest_mem.ps1 -TEST string_append_cont -OPT 1

.\autotest_mem.ps1 -TEST prepend -OPT 1
.\autotest_mem.ps1 -TEST prepend_cont -OPT 1

.\autotest_mem.ps1 -TEST insert_middle -OPT 1
.\autotest_mem.ps1 -TEST insert_middle_cont -OPT 1

echo "Finished!"
