add_executable(gfaster-gui
    main.cpp
    filesystemmodel.cpp
    filesystemmodel.h
)

target_link_libraries(gfaster-gui
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

install(TARGETS gfaster-gui ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
