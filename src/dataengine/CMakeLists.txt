
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
