param([Parameter(Mandatory)][string]$ARCH)

$VSINSTALLDIR = "C:\Program Files\Microsoft Visual Studio\2022\Preview\"
$VS170COMNTOOLS = "C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\Tools\";

if (-not $env:VSINSTALLDIR) {
    $env:VSINSTALLDIR = $VSINSTALLDIR;
}
if (-not $env:VS170COMNTOOLS) {
    $env:VS170COMNTOOLS = $VS170COMNTOOLS;
}

Import-Module "$VS170COMNTOOLS\Microsoft.VisualStudio.DevShell.dll"

$PWD = Get-Location
# -SkipAutomaticLocation
Enter-VsDevShell -VsInstallPath "$env:VSINSTALLDIR" -DevCmdArguments "-arch=$ARCH"

cd $PWD
