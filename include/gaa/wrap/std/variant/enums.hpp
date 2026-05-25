// GAA_VARIANT_register((double), Double) GAA_VARIANT_register((std::string),
//                                                             String)
//     GAA_VARIANT_register((int), Integer) GAA_VARIANT_register((char),
//     Character)
//         GAA_VARIANT_register((std::vector<double>), Vec_Double)
//             GAA_VARIANT_register((std::vector<std::string>), Vec_String)
//                 GAA_VARIANT_register((std::vector<int>), Vec_Integer)
//                     GAA_VARIANT_register((std::vector<char>), Vec_Character)

#define GAA_VARIANT_register_with_vec(TYPE, ENUM)                              \
  GAA_VARIANT_register(TYPE, ENUM)                                             \
      GAA_VARIANT_register((std::vector<GAA_PP_STRIP_PARAM TYPE>), Vec_##ENUM)

#include <gaa/wrap/std/variant/enums_impl.hpp>

#undef GAA_VARIANT_register_with_vec