find_program(LATEX_MK NAMES latexmk)

if(NOT LATEX_MK)
    message(FATAL_ERROR "latexmk not found")
else()
    message(STATUS "Found latexmk: ${LATEX_MK}")
endif()

set(DOCS_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/libGAA.pdf)
set(DOCS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/libGAA.tex)

add_custom_command(
    OUTPUT ${DOCS_OUTPUT}
    COMMAND ${LATEX_MK} -xelatex -output-directory=${CMAKE_CURRENT_SOURCE_DIR} ${DOCS_SRC}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${DOCS_SRC}
    COMMENT "Generating main.pdf with latexmk (XeLaTeX)"
    VERBATIM
)

add_custom_target(gaa_docs DEPENDS ${DOCS_OUTPUT})
add_dependencies(gaa gaa_docs)