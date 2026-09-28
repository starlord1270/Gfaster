add_executable(gfaster-app
    main.cpp
    filesystemmodel.cpp
    filesystemmodel.h
)

target_link_libraries(gfaster-app
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        KF6::KIOWidgets
)

install(TARGETS gfaster-app ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
