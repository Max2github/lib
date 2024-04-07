#include "../../../CPP/smartstring/smartstring.hpp"
#include "../../../CPP/smartbuffer/smartbuffer.hpp"

#define INCLUDE_CPP_SOURCES 1

#ifdef INCLUDE_C_SOURCES
#include "../../../src/smartbuffer/smartbuffer.c"
#include "../../../src/smartbuffer/sBuffer_single.c"
#include "../../../src/smartstring/smartstring.c"
#endif

#ifdef INCLUDE_CPP_SOURCES
#include "../../../CPP/smartbuffer/smartbuffer.cpp"
#include "../../../CPP/smartstring/smartstring.cpp"
#endif