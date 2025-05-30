cmake_minimum_required(VERSION 3.16)

project(SlashGames VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
        main.cpp

        Windows/Main/mainwindow.h
        Windows/Main/mainwindow.cpp
        Windows/Main/mainwindow.ui

        Windows/FaQ/faqwindow.h
        Windows/FaQ/faqwindow.cpp
        Windows/FaQ/faqwindow.ui

        Style/customstyle.h
        Style/customstyle.cpp

        Windows/Authorization/authorizationwindow.h
        Windows/Authorization/authorizationwindow.cpp
        Windows/Authorization/authorizationwindow.ui

        resources/resources.qrc

        Windows/GameInput/gameinputdialog.h
        Windows/GameInput/gameinputdialog.cpp
        Windows/GameInput/gameinputdialog.ui

        Database/databasemanagement.h
        Database/databasemanagement.cpp

        Windows/Custom/customwindow.h
        Windows/Custom/customwindow.cpp
        Windows/Custom/customwindow.ui

        GameManager/gamemanager.h
        GameManager/gamemanager.cpp

        Windows/Balance/balancewindow.h
        Windows/Balance/balancewindow.cpp
        Windows/Balance/balancewindow.ui

)



source_group("MainWindow" FILES
    Windows/Main/mainwindow.h
    Windows/Main/mainwindow.cpp
    Windows/Main/mainwindow.ui
)

source_group("BalanceWindow" FILES
    Windows/Balance/balancewindow.h
    Windows/Balance/balancewindow.cpp
    Windows/Balance/balancewindow.ui
)

source_group("FaQ" FILES
    Windows/FaQ/faqwindow.h
    Windows/FaQ/faqwindow.cpp
    Windows/FaQ/faqwindow.ui
)

source_group("Authorization" FILES
    Windows/Authorization/authorizationwindow.h
    Windows/Authorization/authorizationwindow.cpp
    Windows/Authorization/authorizationwindow.ui
)

source_group("GameInput" FILES
    Windows/GameInput/gameinputdialog.h
    Windows/GameInput/gameinputdialog.cpp
    Windows/GameInput/gameinputdialog.ui
)

source_group("Database" FILES
    Database/databasemanagement.h
    Database/databasemanagement.cpp
)

source_group("GameManager" FILES
    GameManager/gamemanager.h
    GameManager/gamemanager.cpp
)

source_group("Resources" FILES
    resources/resources.qrc
    Style/customstyle.h
    Style/customstyle.cpp
    Windows/Custom/customwindow.h
    Windows/Custom/customwindow.cpp
    Windows/Custom/customwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SlashGames
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SlashGames APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SlashGames SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SlashGames
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SlashGames PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
target_include_directories(SlashGames PRIVATE ${CMAKE_SOURCE_DIR})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SlashGames)
endif()
set_target_properties(SlashGames PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SlashGames
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SlashGames)
endif()
