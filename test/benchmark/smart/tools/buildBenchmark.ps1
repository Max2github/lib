param([Parameter(Mandatory)][string]$ARCH, [string]$Config="Release")

cd benchmark
cmake -S . -B build/ -A $ARCH -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=true
cmake --build "build" --config $Config

mkdir ../../../../../build/benchmark/$ARCH/ | Out-Null
cp build\src\$Config\* ../../../../../build/benchmark/$ARCH/
