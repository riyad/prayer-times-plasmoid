# Project Needs a name ofcourse
project(plasma-prayertimes)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

find_package(ITL REQUIRED)
find_package(Marble REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${KDE4_INCLUDES}
  ${ITL_INCLUDE_DIR}
  ${MARBLE_INCLUDE_DIR})

# We include the data engine
add_subdirectory(dataengine)

# We add our source code here
set(prayertimes_SRCS
    compassgraphicswidget.cpp
    prayertimes.cpp
    prayertimesview.cpp
    prayertimedelegate.cpp)

# Adding configuration ui files
kde4_add_ui_files(prayertimes_SRCS
                  prayertimesLocationConfig.ui
                  prayertimesCalculationMethodConfig.ui)

kde4_add_plugin(plasma_applet_prayertimes ${prayertimes_SRCS})
target_link_libraries(plasma_applet_prayertimes
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_PLASMA_LIBS}
                      ${ITL_LIBRARY}
                      ${MARBLE_LIBRARIES})

# Now make sure all files get to the right place
install(TARGETS plasma_applet_prayertimes
        DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-prayertimes.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

kde4_install_icons(${ICON_INSTALL_DIR})

install(FILES prayertimes.svgz
        DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/prayertimes/)
install(FILES compass.svgz
        DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
