#if !defined(GAA_IN_BUILDING) && !defined(GAA_IN_DEV) && !defined(GAA_IN_TEST)
#if !defined(_MSC_VER)
#warning                                                                       \
    "Any header include this one is currently incomplete, may be complete in the feature"
#else
#pragma message(                                                               \
    "Any header include this one is currently incomplete, may be complete in the feature")
#endif
#endif