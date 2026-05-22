include(GenerateExportHeader)

generate_export_header(gaa
    BASE_NAME GAA
    EXPORT_MACRO_NAME ${GAA_EXPORT_MACRO_NAME}
    EXPORT_FILE_NAME ${GAA_ROOT}/include/gaa/core/buildsystem/export.hpp
)