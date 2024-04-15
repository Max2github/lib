param([Parameter(Mandatory)][string]$ARCH)

$PWD = Get-Location
# -SkipAutomaticLocation
Enter-VsDevShell -VsInstallPath "$env:VSINSTALLDIR" -DevCmdArguments "-arch=$ARCH"

cd $PWD
