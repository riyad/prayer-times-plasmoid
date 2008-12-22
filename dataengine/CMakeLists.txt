add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

set(prayertimes_engine_SRCS
    plasma-engine-prayertimes.cpp
)

kde4_add_plugin(plasma_engine_prayertimes ${prayertimes_engine_SRCS})
target_link_libraries(plasma_engine_prayertimes ${KDE4_KDECORE_LIBS} plasma  ${ITL_LIBRARY})

install(TARGETS plasma_engine_prayertimes DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-prayertimes.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
