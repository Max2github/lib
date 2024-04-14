param([Parameter(Mandatory)][string]$ARCH, [Parameter(Mandatory)][string]$TEST, [string]$Config="Release", [string] $CXXFLAGS = "")

if ($ARCH -ne "x86" -and $ARCH -ne "x64" -and $ARCH -ne "ARM64") {
    Write-Error "ARCH '$ARCH' not supported!"
    exit
}

$OPTFLAGS = 
if ($Config -eq "Release") {
    $OPTFLAGS = "-O3"
} elseif ($Config -eq "Debug") {
    $OPTFLAGS = "-g"
}

. ..\..\..\make_clang_alias.ps1

&"clang-$ARCH" -std=c++14 -Wall -Wextra "$OPTFLAGS" "$CXXFLAGS" -o "bin/$TEST.exe" "$TEST.cpp" "-L../../../build/benchmark/$ARCH" -L../../../build -isystem .\tools\benchmark\include\ -lsmartstring -lsmartbuffer -lword_pick -lwords -lbenchmark -lshlwapi

cp "..\..\..\build\benchmark\$ARCH\benchmark.dll" bin/
