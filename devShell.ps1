param([Parameter(Mandatory)][string]$ARCH)

Enter-VsDevShell -VsInstallPath "$env:VSINSTALLDIR" -SkipAutomaticLocation -DevCmdArguments "-arch=$ARCH"