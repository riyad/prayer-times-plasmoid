# Project Needs a name ofcourse
project(plasma-applet-prayertimes)

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
add_subdirectory(src)

# Now make sure all files get to the right place
kde4_install_icons(${ICON_INSTALL_DIR})

install(FILES plasma-applet-prayertimes.notifyrc
        DESTINATION ${DATA_INSTALL_DIR}/plasma-applet-prayertimes)
