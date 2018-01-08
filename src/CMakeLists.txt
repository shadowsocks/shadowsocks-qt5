set(APP_NAME "ss-qt5")
add_definitions(-DAPP_VERSION="${PROJECT_VERSION}")

set(SOURCE
    icons.qrc
    translations.qrc
    main.cpp
    mainwindow.cpp
    ip4validator.cpp
    portvalidator.cpp
    ssvalidator.cpp
    qrwidget.cpp
    sharedialog.cpp
    editdialog.cpp
    connection.cpp
    confighelper.cpp
    urihelper.cpp
    uriinputdialog.cpp
    sqprofile.cpp
    settingsdialog.cpp
    statusnotifier.cpp
    connectiontablemodel.cpp
    connectionitem.cpp
    qrcodecapturer.cpp
    )

if (WIN32)
    list(APPEND SOURCE ss-qt5.rc)
endif()

add_executable(${APP_NAME} WIN32 ${SOURCE})

target_link_libraries(${APP_NAME}
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Widgets
    PUBLIC Qt5::Network
    PRIVATE ${QSS_LIBRARY_VAR}
    PRIVATE ${QRENCODE_LIBRARY_VAR}
    PRIVATE ${ZBAR_LIBRARY_VAR})
target_include_directories(${APP_NAME}
    PRIVATE ${QSS_INCLUDE_DIRS}
    PRIVATE ${QRENCODE_INCLUDE_DIRS}
    PRIVATE ${ZBAR_INCLUDE_DIRS})

if (UNIX AND NOT APPLE)
    target_link_libraries(${APP_NAME} PRIVATE Qt5::DBus)
endif()

install(TARGETS ${APP_NAME}
    RUNTIME DESTINATION bin)

if (UNIX)
    install(FILES shadowsocks-qt5.desktop DESTINATION share/applications)
    install(FILES icons/shadowsocks-qt5.png DESTINATION share/icons/hicolor/512x512/apps)
endif()
