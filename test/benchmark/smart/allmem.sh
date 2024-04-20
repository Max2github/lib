echo "Building libraries..."

cd ../../../
rm -f build/*.a build/obj/*.o build/obj/*/*.o
make words word_pick #smartbuffer smartstring
cd test/benchmark/smart/

echo "Compiling Tests..."

make memtest TEST=string_append OPT=1
make memtest TEST=string_append_cont OPT=1

make memtest TEST=prepend OPT=1
make memtest TEST=prepend_cont OPT=1

make memtest TEST=insert_middle OPT=1
make memtest TEST=insert_middle_cont OPT=1

echo "Running Tests..."

make autotest_mem TEST=string_append OPT=1
make autotest_mem TEST=string_append_cont OPT=1

make autotest_mem TEST=prepend OPT=1
make autotest_mem TEST=prepend_cont OPT=1

make autotest_mem TEST=insert_middle OPT=1
make autotest_mem TEST=insert_middle_cont OPT=1

echo "Finished!"
