param([Parameter(Mandatory)][string]$ARCH, [Parameter(Mandatory)][string]$TEST, [string]$Config="Release", [string[]] $CXXFLAGS)

if ($ARCH -ne "x86" -and $ARCH -ne "x64" -and $ARCH -ne "ARM64") {
    Write-Error "ARCH '$ARCH' not supported!"
    exit
}

$OPTFLAGS = ""
if ("$Config" -eq "Release") {
    $OPTFLAGS = "-O3"
}
if ("$Config" -eq "Debug") {
    $OPTFLAGS = "-g"
}

$INCLUDE_C_SOURCES = "$CXXFLAGS" | Select-String -Pattern '-DINCLUDE_C_SOURCES'

$LIBS = "-lword_pick -lwords -lbenchmark -lshlwapi"
$LIB_WORD_PICK = '-lword_pick'
$LIB_WORDS     = '-lwords'
$LIB_BENCHMARK = '-lbenchmark'
$LIB_SHLWAPI   = '-lshlwapi'
$LIB_SMARTBUFFER = $null
$LIB_SMARTSTRING = $null
if ($INCLUDE_C_SOURCES -eq $null) {
    $LIB_SMARTBUFFER = '-lsmartbuffer'
    $LIB_SMARTSTRING = '-lsmartstring'
}

. ..\..\..\make_clang_alias.ps1

echo "clang-$ARCH -std=c++14 -Wall -Wextra $OPTFLAGS $CXXFLAGS -o bin/$TEST.exe $TEST.cpp -L../../../build/benchmark/$ARCH -L../../../build -isystem .\tools\benchmark\include\ $LIB_SMARTSTRING $LIB_SMARTBUFFER $LIB_WORD_PICK $LIB_WORDS $LIB_BENCHMARK $LIB_SHLWAPI"

&"clang-$ARCH" -std=c++14 -Wall -Wextra "$OPTFLAGS" "$CXXFLAGS" -o "bin/$TEST.exe" "$TEST.cpp" "-L../../../build/benchmark/$ARCH" -L../../../build -isystem .\tools\benchmark\include\ $LIB_SMARTSTRING $LIB_SMARTBUFFER $LIB_WORD_PICK $LIB_WORDS $LIB_BENCHMARK $LIB_SHLWAPI

cp "..\..\..\build\benchmark\$ARCH\benchmark.dll" bin/
