function autotest_mkdir {
    param (
        [Parameter(Mandatory)][string]$BINDIR,
        [Parameter(Mandatory)]$TEST,
        [Parameter(Mandatory)][bool]$OPT
    )

    $OS_UNAME = "Windows"
    $RESULT_DIR = "results"

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

    write-host "$AUTOTEST_RESULTS_DIR"

    New-Item -Force -Path "$AUTOTEST_RESULTS_DIR" -ItemType "directory" | Out-Null

    return $AUTOTEST_RESULTS_DIR;
}
