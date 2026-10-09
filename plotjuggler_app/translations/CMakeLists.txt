# Translation resources are compiled on normal builds; only the explicit update
# target rewrites the checked-in editable catalogue.
find_package(Qt5 REQUIRED COMPONENTS LinguistTools)

set(PJ_TRANSLATION_TS "${CMAKE_CURRENT_SOURCE_DIR}/plotjuggler_zh_CN.ts")
set(PJ_QT_TRANSLATION_TS "${CMAKE_CURRENT_SOURCE_DIR}/qt/qt_zh_CN.ts")
qt5_add_translation(PJ_TRANSLATION_QM
    ${PJ_TRANSLATION_TS} ${PJ_QT_TRANSLATION_TS}
    OPTIONS -nounfinished)

file(TO_CMAKE_PATH "${CMAKE_CURRENT_BINARY_DIR}" PJ_QM_RESOURCE_ROOT)
configure_file(plotjuggler_i18n.qrc.in plotjuggler_i18n.qrc @ONLY)
qt5_add_resources(PJ_TRANSLATION_RESOURCE
    "${CMAKE_CURRENT_BINARY_DIR}/plotjuggler_i18n.qrc")
add_library(plotjuggler_translations STATIC ${PJ_TRANSLATION_RESOURCE} ${PJ_TRANSLATION_QM})
target_link_libraries(plotjuggler_translations PUBLIC Qt5::Core)

set(PJ_TRANSLATION_SOURCES)
set(PJ_TRANSLATION_INCLUDE_FLAGS)
foreach(module plotjuggler_app plotjuggler_base plotjuggler_plugins)
  file(GLOB_RECURSE module_sources CONFIGURE_DEPENDS
      "${PROJECT_SOURCE_DIR}/${module}/*.cpp"
      "${PROJECT_SOURCE_DIR}/${module}/*.cc"
      "${PROJECT_SOURCE_DIR}/${module}/*.cxx"
      "${PROJECT_SOURCE_DIR}/${module}/*.h"
      "${PROJECT_SOURCE_DIR}/${module}/*.hpp"
      "${PROJECT_SOURCE_DIR}/${module}/*.ui")
  list(FILTER module_sources EXCLUDE REGEX "/(3rdparty|tests|test|demos|demo|build[^/]*|translations|rosx_introspection)/")
  list(APPEND PJ_TRANSLATION_SOURCES ${module_sources})
  foreach(include_subdir "" src include)
    if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/${module}/${include_subdir}")
      list(APPEND PJ_TRANSLATION_INCLUDE_FLAGS "-I${PROJECT_SOURCE_DIR}/${module}/${include_subdir}")
    endif()
  endforeach()
endforeach()
set(PJ_LUPDATE_INPUT "")
foreach(source IN LISTS PJ_TRANSLATION_SOURCES)
  # Qt 5 response files treat the complete line as a filename, including spaces.
  # Quotes become literal filename characters in this format.
  string(APPEND PJ_LUPDATE_INPUT "${source}\n")
endforeach()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/lupdate-sources.txt" "${PJ_LUPDATE_INPUT}")
add_custom_target(pj_update_translations
    COMMAND Qt5::lupdate "@${CMAKE_CURRENT_BINARY_DIR}/lupdate-sources.txt"
        ${PJ_TRANSLATION_INCLUDE_FLAGS} -no-obsolete -locations relative
        -source-language en_US -target-language zh_CN -ts ${PJ_TRANSLATION_TS}
    VERBATIM)

install(DIRECTORY qt/ DESTINATION share/plotjuggler/translations/qt)
