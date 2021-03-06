project(qnut)

find_package(Qt4 COMPONENTS QtCore QtGui QtDbus QtNetwork REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/utils ${CMAKE_SOURCE_DIR})
include(${QT_USE_FILE})

set(INSTALL_SHARE_PREFIX ${CMAKE_INSTALL_PREFIX}/share)
set(INSTALL_SHARE_TARGET_PREFIX ${INSTALL_SHARE_PREFIX}/${PROJECT_NAME})

set(FORMS
	ui/devicedetails.ui
	ui/ipconfiguration.ui
	ui/devicesettings.ui
	ui/wirelesssettings.ui
	ui/accesspointconfig.ui
	ui/adhocconfig.ui
)

set(SOURCES_MOC_H
	cconnectionmanager.h
	cipconfiguration.h
	cwirelesssettings.h
	cabstractwifinetconfigdialog.h
	caccesspointconfig.h
	cadhocconfig.h
	cdevicesettings.h
	cuidevice.h
	cnotificationmanager.h
	
	modelview/cuidevicemodel.h
	modelview/cenvironmenttreemodel.h
	modelview/cinterfacedetailsmodel.h
	modelview/cenvironmentdetailsmodel.h
	modelview/cavailableapmodel.h
	modelview/cmanagedapmodel.h
	modelview/cdnslistmodel.h
	modelview/cipeditdelegate.h
	modelview/ccommandlistmodel.h
)

set(SOURCES_CPP main.cpp
	common.cpp
	
	cconnectionmanager.cpp
	cipconfiguration.cpp
	cwirelesssettings.cpp
	cabstractwifinetconfigdialog.cpp
	caccesspointconfig.cpp
	cadhocconfig.cpp
	cdevicesettings.cpp
	cuidevice.cpp
	cnotificationmanager.cpp
	
	modelview/cuidevicemodel.cpp
	modelview/cenvironmenttreemodel.cpp
	modelview/cinterfacedetailsmodel.cpp
	modelview/cenvironmentdetailsmodel.cpp
	modelview/cavailableapmodel.cpp
	modelview/cmanagedapmodel.cpp
	modelview/cdnslistmodel.cpp
	modelview/cipeditdelegate.cpp
	modelview/ccommandlistmodel.cpp
	
	utils/cerrorcodeevaluator.cpp
)

set(TRANSLATIONS_TS
	translations/${PROJECT_NAME}_de.ts
	translations/${PROJECT_NAME}_pt_BR.ts
)

qt4_wrap_ui(FORMS_H ${FORMS})
qt4_wrap_cpp(SOURCES_MOC_CPP ${SOURCES_MOC_H})
qt4_add_translation(TRANSLATIONS_QM ${TRANSLATIONS_TS})

add_executable(${PROJECT_NAME}
	${SOURCES_CPP}
	${SOURCES_MOC_CPP}
	${SOURCES_H}
	${FORMS_H}
	${TRANSLATIONS_QM}
)

add_target_properties(${PROJECT_NAME} COMPILE_FLAGS "-DUI_VERSION='\"${PACKAGE_VERSION}\"'")

target_link_libraries(${PROJECT_NAME}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	-L${nutclient_BINARY_DIR}
	-lnutclient
)

file(GLOB RES_ICONS res/*.png res/qnut.svg res/qnut_small.svg)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${TRANSLATIONS_QM} DESTINATION ${INSTALL_SHARE_TARGET_PREFIX}/lang)
install(FILES ${RES_ICONS} DESTINATION ${INSTALL_SHARE_TARGET_PREFIX}/icons)
install(FILES ${PROJECT_NAME}.desktop DESTINATION ${INSTALL_SHARE_PREFIX}/applications)
