# Project Needs a name ofcourse
project(plasma-prayertimes)
 

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)
find_package(ITL REQUIRED)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )
 
# We add our source code here
set(prayertimes_SRCS prayertimes.cpp)
 
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_prayertimes ${prayertimes_SRCS})
target_link_libraries(plasma_applet_prayertimes 
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
 
install(TARGETS plasma_applet_prayertimes
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-prayertimes.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
