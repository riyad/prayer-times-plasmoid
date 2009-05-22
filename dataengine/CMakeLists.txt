
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${KDE4_INCLUDES}
  ${ITL_INCLUDE_DIR}
  ${MARBLE_INCLUDE_DIR}
  )

set(prayertimes_engine_SRCS prayertimesengine.cpp)

kde4_add_plugin(plasma_engine_prayertimes ${prayertimes_engine_SRCS})
target_link_libraries(plasma_engine_prayertimes
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_PLASMA_LIBS}
                      ${ITL_LIBRARY}
                      ${MARBLE_LIBRARIES})

install(TARGETS plasma_engine_prayertimes
        DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-prayertimes.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
