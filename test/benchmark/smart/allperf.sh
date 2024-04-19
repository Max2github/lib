echo "Building libraries..."

cd ../../../
rm -f build/*.a build/obj/*.o build/obj/*/*.o
make words word_pick smartbuffer smartstring
cd test/benchmark/smart/

echo "Compiling Tests..."

make build TEST=string_append OPT=1
make build TEST=string_append_cont OPT=1

make build TEST=prepend OPT=1
make build TEST=prepend_cont OPT=1

make build TEST=insert_middle OPT=1
make build TEST=insert_middle_cont OPT=1

make build TEST=construct OPT=1

make build TEST=iterate OPT=1
make build TEST=iterate_fragmentation OPT=1

echo "Running Tests..."

make autotest TEST=string_append OPT=1
make autotest TEST=string_append_cont OPT=1

make autotest TEST=prepend OPT=1
make autotest TEST=prepend_cont OPT=1

make autotest TEST=insert_middle_cont OPT=1
make autotest TEST=insert_middle_cont OPT=1

make autotest TEST=construct OPT=1

make autotest TEST=iterate OPT=1
make autotest TEST=iterate_fragmentation OPT=1

echo "Finished!"
