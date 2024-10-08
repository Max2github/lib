param ([Parameter(Mandatory)][string]$TEST, [Parameter(Mandatory)][bool]$OPT)

#$PWD = Get-Location | Foreach-Object { $_.Path }
$BINDIR = "bin"

. .\tools\autotest_mkdir.ps1

$AUTOTEST_RESULTS_DIR = autotest_mkdir -BINDIR "$BINDIR" -TEST "$TEST\mem" -OPT $OPT

&"$PWD\$BINDIR\$TEST.exe" --benchmark_out=$AUTOTEST_RESULTS_DIR\result.json --benchmark_out_format=json

.\tools\memtestJson.ps1 -JSONRESULT $AUTOTEST_RESULTS_DIR\result.json
