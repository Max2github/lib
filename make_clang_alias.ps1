$VCINSTALLDIR = $env:VCINSTALLDIR

set-alias -Name clang-x64 -Value "$VCINSTALLDIR\Tools\Llvm\x64\bin\clang.exe"
set-alias -Name clang-x86 -Value "$VCINSTALLDIR\Tools\Llvm\bin\clang.exe"
set-alias -Name clang-ARM64 -Value "$VCINSTALLDIR\Tools\Llvm\ARM64\bin\clang.exe"
