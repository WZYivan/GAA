#define GAA_VARIANT_register_compact(TYPE, ENUM)                               \
  GAA_VARIANT_register(TYPE, ENUM) GAA_VARIANT_register(                       \
      (std::vector<GAA_PP_STRIP_PARAM TYPE>), Vec_##ENUM)                      \
      GAA_VARIANT_register((std::map<std::size_t, GAA_PP_STRIP_PARAM TYPE>),   \
                           Map_Idx_##ENUM)                                     \
          GAA_VARIANT_register(                                                \
              (std::map<std::string, GAA_PP_STRIP_PARAM TYPE>),                \
              Map_Str_##ENUM)

#include <gaa/wrap/std/variant/enums_impl.hpp>

#undef GAA_VARIANT_register_with_vec