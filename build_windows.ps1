param([Parameter(Mandatory)][string]$ARCH)

if ($ARCH -ne "x86" -and $ARCH -ne "x64" -and $ARCH -ne "ARM64") {
    Write-Error "ARCH '$ARCH' not supported!"
    exit
}

. .\make_clang_alias.ps1

#Remove-Item -Force .\build\*.lib
#Remove-Item -Force .\build\obj\*.o
#Remove-Item -Force build\obj\*\*.o

&"clang-$ARCH" -c -x c -Wall -Wextra -Wno-missing-braces -O3 -o build/obj/words.o src/words.h
&"clang-$ARCH" -c -x c -Wall -Wextra -Wno-missing-braces -O3 -o build/obj/word_pick.o src/word_pick.h
&"clang-$ARCH" -c -x c -Wall -Wextra -Wno-missing-braces -O3 -o build/obj/list.o src/list.h
&"clang-$ARCH" -c -x c -Wall -Wextra -Wno-missing-braces -O3 -o build/obj/smartbuffer/sBuffer_single.o src/smartbuffer/sBuffer_single.c
&"clang-$ARCH" -c -x c -Wall -Wextra -Wno-missing-braces -O3 -o build/obj/smartbuffer/smartbuffer.o src/smartbuffer/smartbuffer.c
&"clang-$ARCH" -c -x c -Wall -Wextra -Wno-missing-braces -O3 -o build/obj/smartstring/smartstring.o src/smartstring/smartstring.c

llvm-lib /out:build\words.lib build/obj/words.o
llvm-lib /out:build\word_pick.lib build/obj/word_pick.o
llvm-lib /out:build\list.lib build/obj/list.o
llvm-lib /out:build\smartbuffer.lib build/obj/smartbuffer/sBuffer_single.o build/obj/smartbuffer/smartbuffer.o
llvm-lib /out:build\smartstring.lib build/obj/smartstring/smartstring.o