cmake_minimum_required(VERSION 3.19)
set(QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
project(EVStereonet LANGUAGES CXX)

# 查找 Qt（必须在 qt_standard_project_setup 之前）
find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools)

# 启用 Qt 标准项目设置（自动处理 AUTOMOC, AUTOUIC, AUTORCC 等）
qt_standard_project_setup()

# === 平台特定编译选项（仅影响编译器标志，可提前设置）===
if(WIN32)
    message(STATUS "Building for Windows")
    add_compile_options("/utf-8")
    add_definitions(-DUNICODE -D_UNICODE)

elseif(APPLE)
    message(STATUS "Building for macOS")

elseif(ANDROID)
    message(STATUS "Building for Android")
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
    set(ANDROID_ABIS "armeabi-v7a;arm64-v8a;x86;x86_64")  # 注意：用分号分隔

elseif(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly")
    add_compile_options(
        "-finput-charset=UTF-8"
        "-fexec-charset=UTF-8"
    )
    add_definitions(-DQT_USE_UTF8)

elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    add_compile_options(-fPIC)
endif()

# === 创建可执行文件（关键：必须在此处定义目标）===
qt_add_executable(EVStereonet
    WIN32
    MACOSX_BUNDLE
    main.cpp
    stereonet.h
    stereonet.cpp
    stereonetwidget.h
    stereonetwidget.cpp
)

# === 添加翻译 ===
qt_add_translations(
    TARGETS EVStereonet
    TS_FILES EVStereonet_zh_CN.ts
    LUPDATE_OPTIONS -no-obsolete
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)

# === 设置目标属性（现在 EVStereonet 已存在！）===
set_target_properties(EVStereonet PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON

    # macOS
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.EVStereonet"
    MACOSX_BUNDLE_BUNDLE_NAME "EVStereonet"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"

    # Android
    ANDROID_PACKAGE_NAME "com.example.helloworld"
    ANDROID_VERSION_CODE "1"
    ANDROID_VERSION_NAME "1.0"
)

# === WebAssembly 特定属性（安全：目标已存在）===
if(EMSCRIPTEN)
    set_target_properties(EVStereonet PROPERTIES
        QT_WASM_ENABLE_EXCEPTION_CATCHING ON
        QT_WASM_LOAD_LOCAL_FILES ON
        QT_WASM_REQUIRE_PREFIX ""
    )
endif()

# === 链接库 ===
target_link_libraries(EVStereonet PRIVATE
    Qt::Core
    Qt::Widgets
)

# Android 额外组件（按需链接）
if(ANDROID)
    find_package(Qt6 REQUIRED COMPONENTS )  #AndroidExtras
    #target_link_libraries(EVStereonet PRIVATE Qt::AndroidExtras)
endif()

# === 安装与部署 ===
include(GNUInstallDirs)

install(TARGETS EVStereonet
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(QM_FILES)
    install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/translations)
endif()

# 部署脚本
if(NOT ANDROID)
    qt_generate_deploy_app_script(
        TARGET EVStereonet
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

# if(ANDROID)
#     include(qt_add_apk)
#     qt_add_apk(HelloWorldApk EVStereonet
#         PACKAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/android
#     )
# endif()

# === Android 打包配置（Qt 6.5+ 自动处理 APK 生成）===
if(ANDROID)
    # 设置 Android 包信息（已在 set_target_properties 中设置）
    # 指定 android/ 目录作为 Gradle 项目源
    set_target_properties(EVStereonet PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
endif()
