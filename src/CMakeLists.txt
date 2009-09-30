
# We include the data engine
add_subdirectory(dataengine)

# We add our source code here
set(prayertimes_SRCS
    compassgraphicswidget.cpp
    prayertimesapplet.cpp
    prayertimesview.cpp
    prayertimedelegate.cpp)

# Adding configuration ui files
kde4_add_ui_files(prayertimes_SRCS
                  prayertimesNotificationsConfig.ui
                  prayertimesLocationConfig.ui
                  prayertimesCalculationMethodConfig.ui)

kde4_add_plugin(plasma_applet_prayertimes ${prayertimes_SRCS})
target_link_libraries(plasma_applet_prayertimes
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_PLASMA_LIBS}
                      ${MARBLE_LIBRARIES})

# Now make sure all files get to the right place
install(TARGETS plasma_applet_prayertimes
        DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-prayertimes.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES prayertimes.svgz
        DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/prayertimes/)
install(FILES compass.svgz
        DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
