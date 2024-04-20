param ([Parameter(Mandatory)][string]$TEST, [Parameter(Mandatory)][bool]$OPT)

#$PWD = Get-Location | Foreach-Object { $_.Path }
$BINDIR = "bin"

. .\tools\autotest_mkdir.ps1

$AUTOTEST_RESULTS_DIR = autotest_mkdir -BINDIR "$BINDIR" -TEST "$TEST" -OPT $OPT

&"$PWD\$BINDIR\$TEST.exe" --benchmark_out=$AUTOTEST_RESULTS_DIR\console.txt --benchmark_out_format=console
&"$PWD\$BINDIR\$TEST.exe" --benchmark_out=$AUTOTEST_RESULTS_DIR\result.json --benchmark_out_format=json
#&"$PWD\$BINDIR\$TEST.exe" --benchmark_out=$AUTOTEST_RESULTS_DIR\result.csv  --benchmark_out_format=csv

#Copy-Item $AUTOTEST_RESULTS_DIR\result.json result_temp.json

# execute plotJson.ps
