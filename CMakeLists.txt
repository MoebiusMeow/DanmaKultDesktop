# Generated from DanmakuItDesktop.pro.

cmake_minimum_required(VERSION 3.16)
project(DanmakuItDesktop LANGUAGES CXX)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OPENSSL_ROOT_DIR /opt/openssl@3)
	set(CMAKE_PREFIX_PATH /usr/local/Qt-6.2.2)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Network)
find_package(Qt6 COMPONENTS OpenGL)
find_package(Qt6 COMPONENTS OpenGLWidgets)
find_package(Qt6 COMPONENTS WebSockets)

set(SOURCES
        Network/NetworkAPI.cpp Network/NetworkAPI.h
        Network/NetworkConfig.h
        ScreenOverlay/DanmakuConstants.h
        ScreenOverlay/DanmakuTestSet.cpp ScreenOverlay/DanmakuTestSet.h
        ScreenOverlay/DanmakuText.cpp ScreenOverlay/DanmakuText.h
        ScreenOverlay/DanmakuTextBottom.cpp ScreenOverlay/DanmakuTextBottom.h
        ScreenOverlay/DanmakuTextBottomSet.cpp ScreenOverlay/DanmakuTextBottomSet.h
        ScreenOverlay/DanmakuTextFloat.cpp ScreenOverlay/DanmakuTextFloat.h
        ScreenOverlay/DanmakuTextFloatSet.cpp ScreenOverlay/DanmakuTextFloatSet.h
        ScreenOverlay/DanmakuTextSet.cpp ScreenOverlay/DanmakuTextSet.h
        ScreenOverlay/DanmakuTextTop.cpp ScreenOverlay/DanmakuTextTop.h
        ScreenOverlay/DanmakuTextTopSet.cpp ScreenOverlay/DanmakuTextTopSet.h
        ScreenOverlay/DanmakuWidget.cpp ScreenOverlay/DanmakuWidget.h
        View/DynamicVBoxLayout.cpp View/DynamicVBoxLayout.h
        View/KultLoginBox.cpp View/KultLoginBox.h
        View/KultMessageBox.cpp View/KultMessageBox.h
        View/MainWindow.cpp View/MainWindow.h
        View/WaveWidget.cpp View/WaveWidget.h
        main.cpp)

# Resources:
set(resources_resource_files
    "Assets/Fonts/NotoColorEmoji_WindowsCompatible.ttf"
    "Assets/Fonts/NotoSansCJKsc-Bold.otf"
    "Assets/Fonts/fontawesome-webfont.ttf"
    "Assets/Icons/logo.png"
    "Assets/Icons/logo_w.png"
    "Assets/Shaders/plainUV.vert"
    "Assets/Shaders/waveShader.frag"
    "Assets/Styles/mainUI.qss"
    "Assets/Certificates/isrgrootx1.pem"
)

qt6_add_big_resources(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX_BUNDLE_ICON_FILE mac_icon.icns)
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Icons/mac_icon.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #disabled, as Qt6 has poor support for rc files
    #enable_language("RC")
    #set(app_icon_windows "${CMAKE_CURRENT_SOURCE_DIR}/DanmakuIt.rc")
endif()

qt_add_executable(DanmakuItDesktop WIN32 ${app_icon_windows} MACOSX_BUNDLE ${app_icon_macos} ${SOURCES})

target_link_libraries(DanmakuItDesktop PUBLIC
        Qt::Core
        Qt::Gui
        Qt::OpenGL
        Qt::OpenGLWidgets
        Qt::WebSockets
        )

if((QT_MAJOR_VERSION GREATER 4))
    target_link_libraries(DanmakuItDesktop PUBLIC
        Qt::Widgets
    )
endif()

install(TARGETS DanmakuItDesktop
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
