find_package(Qt5 REQUIRED COMPONENTS Test LinguistTools)
qt5_add_resources(PJ_LANGUAGE_TEST_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../resource.qrc")
add_executable(language_manager_test
    language_manager_test.cpp ../language_manager.cpp)
target_include_directories(language_manager_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(language_manager_test PRIVATE Qt5::Test plotjuggler_translations)
set_target_properties(language_manager_test PROPERTIES WIN32_EXECUTABLE OFF)
add_test(NAME LanguageManagerTest COMMAND language_manager_test)

add_executable(preferences_language_test
    preferences_language_test.cpp ../preferences_dialog.cpp ../preferences_dialog.h
    ../language_manager.cpp ${PJ_LANGUAGE_TEST_RESOURCES})
target_include_directories(preferences_language_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.." "${PROJECT_SOURCE_DIR}/3rdparty/QCodeEditor/include")
target_link_libraries(preferences_language_test PRIVATE Qt5::Test Qt5::Svg
    plotjuggler_base plotjuggler_translations QCodeEditor)
set_target_properties(preferences_language_test PROPERTIES WIN32_EXECUTABLE OFF)
add_test(NAME PreferencesLanguageTest COMMAND preferences_language_test)

# Explicit Windows UI/real-log acceptance runner (fixture supplied by the operator).
add_executable(native_language_qa native_language_qa.cpp ../preferences_dialog.cpp
    ../preferences_dialog.h ../language_manager.cpp ${PJ_LANGUAGE_TEST_RESOURCES})
target_include_directories(native_language_qa PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${PROJECT_SOURCE_DIR}/3rdparty/QCodeEditor/include")
target_link_libraries(native_language_qa PRIVATE Qt5::Test Qt5::Svg Qt5::Xml
    plotjuggler_base plotjuggler_translations QCodeEditor)
set_target_properties(native_language_qa PROPERTIES WIN32_EXECUTABLE OFF)

find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
  add_test(NAME ChineseTranslationCatalogue COMMAND ${Python3_EXECUTABLE}
      "${PROJECT_SOURCE_DIR}/scripts/validate_translations.py"
      --lupdate $<TARGET_FILE:Qt5::lupdate>)
  add_test(NAME ChineseTranslationValidator COMMAND ${Python3_EXECUTABLE}
      "${PROJECT_SOURCE_DIR}/scripts/test_validate_translations.py")
endif()
