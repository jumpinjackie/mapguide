configure_file(${CMAKE_MODULE_PATH}/../configs/php.ini.in ${CMAKE_CURRENT_BINARY_DIR}/php.ini)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/php.ini DESTINATION ${MG_INSTALL_WEB_PREFIX}/php/lib COMPONENT ${MG_COMPONENT})

configure_file(${CMAKE_MODULE_PATH}/../configs/mapguide.conf.in ${CMAKE_CURRENT_BINARY_DIR}/mapguide.conf)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mapguide.conf DESTINATION ${MG_INSTALL_WEB_PREFIX}/apache2/conf/mapguide COMPONENT ${MG_COMPONENT})

if (WITH_JAVA)
    configure_file(${CMAKE_MODULE_PATH}/../configs/workers.properties.in ${CMAKE_CURRENT_BINARY_DIR}/workers.properties)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/workers.properties DESTINATION ${MG_INSTALL_WEB_PREFIX}/apache2/conf/mapguide COMPONENT ${MG_COMPONENT})

    configure_file(${CMAKE_MODULE_PATH}/../configs/tomcat.conf.in ${CMAKE_CURRENT_BINARY_DIR}/tomcat.conf)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tomcat.conf DESTINATION ${MG_INSTALL_WEB_PREFIX}/apache2/conf/mapguide COMPONENT ${MG_COMPONENT})

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tomcat)
    execute_process(COMMAND tar -zxf ${CMAKE_CURRENT_SOURCE_DIR}/apache-tomcat-7.0.82.tar.gz -C ${CMAKE_CURRENT_BINARY_DIR}/tomcat --strip-components=1)

    configure_file(${CMAKE_MODULE_PATH}/../configs/mapguide.xml.in ${CMAKE_CURRENT_BINARY_DIR}/tomcat/conf/Catalina/localhost/mapguide.xml)
    # Don't overwrite startup.sh in the CMAKE_CURRENT_BINARY_DIR as the subseqent install will do the same thing
    # and we'll get auto chmod+x as a bonus
    configure_file(${CMAKE_MODULE_PATH}/../configs/tomcat_startup.sh.in ${CMAKE_CURRENT_BINARY_DIR}/startup.sh)
    
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tomcat DESTINATION ${MG_INSTALL_WEB_PREFIX})
    file(GLOB TOMCAT_SCRIPTS "${CMAKE_CURRENT_BINARY_DIR}/tomcat/bin/*.sh")
    install(PROGRAMS ${TOMCAT_SCRIPTS} DESTINATION ${MG_INSTALL_WEB_PREFIX}/tomcat/bin)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/startup.sh DESTINATION ${MG_INSTALL_WEB_PREFIX}/tomcat/bin)
endif (WITH_JAVA)