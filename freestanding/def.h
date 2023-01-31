#ifndef NO_STD_LIB
    /**
     * NO_STD_LIB is not defined
     * and we want it to stay that way,
     * but we don not want def.h to include unnecessary stuff.
     * */
    #define NO_STD_LIB
    #include "../templates/def.h"
    #undef NO_STD_LIB
#else
    #include "../templates/def.h"
#endif