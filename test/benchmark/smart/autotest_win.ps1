param ([Parameter(Mandatory)][string]$TEST, [Parameter(Mandatory)][bool]$OPT)

$OS_UNAME = "Windows"
#$PWD = Get-Location | Foreach-Object { $_.Path }
$RESULT_DIR = "results"
$BINDIR = "bin"

$AUTOTEST_RESULTS_DIR = "$RESULT_DIR"

if ($OPT) {
    $AUTOTEST_RESULTS_DIR = "$RESULT_DIR\opt"
} else {
    $AUTOTEST_RESULTS_DIR = "$RESULT_DIR\debug"
}

$timestamp = Get-Date -Format o

$AUTOTEST_RESULTS_DIR = "$AUTOTEST_RESULTS_DIR\$OS_UNAME\$TEST\$timestamp"

# colon (:) not supported in paths on windows :(

$AUTOTEST_RESULTS_DIR = $AUTOTEST_RESULTS_DIR | ForEach-Object { $_ -replace ":", "." }

New-Item -Force -Path "$AUTOTEST_RESULTS_DIR" -ItemType "directory" | Out-Null

&"$PWD\$BINDIR\$TEST.exe" --benchmark_out=$AUTOTEST_RESULTS_DIR\console.txt --benchmark_out_format=console
&"$PWD\$BINDIR\$TEST.exe" --benchmark_out=$AUTOTEST_RESULTS_DIR\result.json --benchmark_out_format=json
&"$PWD\$BINDIR\$TEST.exe" --benchmark_out=$AUTOTEST_RESULTS_DIR\result.csv  --benchmark_out_format=csv

#Copy-Item $AUTOTEST_RESULTS_DIR\result.json result_temp.json

# execute plotJson.ps
