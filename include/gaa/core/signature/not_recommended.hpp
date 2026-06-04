#if !defined(GAA_IN_BUILDING) && !defined(GAA_IN_DEV) && !defined(GAA_IN_TEST)
#if !defined(_MSC_VER)
#warning                                                                       \
    "Any header include this one is not recommended, it may have an alternative"
#else
#pragma message(                                                               \
    "Any header include this one is not recommended, it may have an alternative")
#endif
#endif