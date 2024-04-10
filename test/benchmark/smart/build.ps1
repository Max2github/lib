param([Parameter(Mandatory)][string]$ARCH, [string]$Config="Release")

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

&"clang-$ARCH" -std=c++14 -Wall -Wextra "$OPTFLAGS" -o "bin/string_append_cont.exe" "string_append_cont.cpp" "-L../../../build/benchmark/$ARCH" -L../../../build -isystem .\tools\benchmark\include\ -lsmartstring -lsmartbuffer -lword_pick -lwords -lbenchmark -lshlwapi

cp "..\..\..\build\benchmark\$ARCH\benchmark.dll" bin/
