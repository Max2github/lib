param([Parameter(Mandatory)][string]$TEST)

cd ../../../

rm build/*.a
rm build/obj/*.o
rm build/obj/*/*.o
mingw32-make words word_pick smartbuffer smartstring

cd test/benchmark/smart

#mingw32-make build "TEST=$TEST" OPT=1
#mingw32-make autotest "TEST=$TEST" OPT=1

$OPTFLAGS = "-O3"
#$OPTFLAGS = "-g"

g++ -std=c++11 -Wall -Wextra "$OPTFLAGS" "$CXXFLAGS" -o "bin/$TEST.exe" "$TEST.cpp" "-L../../../build/benchmark/mingw64" -L../../../build -Itools/benchmark/include -isystem tools/benchmark/include -lsmartstring -lsmartbuffer -lword_pick -lwords -lbenchmark


