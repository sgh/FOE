project(FOEClanManager)
cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0020 NEW)

## Config
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

message("Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_COMPILER_IS_GNUCXX)
##-fvisibility=hidden -fvisibility-inlines-hidden
	set(CMAKE_CXX_FLAGS                "-pipe -Wextra -Wall -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -ggdb")
	set(CMAKE_CXX_FLAGS_MINSIZEREL     "-s -Os -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE        "-s -O2 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-s -O2 -g")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find Qt libraries
find_package(Qt5Core    REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

## Library
set(SOURCES
             src/foe_main.cpp
             src/foeclan.cpp
             src/foedatamanger.cpp
             src/foeuser.cpp
             src/foegoods.cpp
             src/foeusereditdlg.cpp
             src/foeages.cpp
             src/foeoverviewmodel.cpp
             src/foepersistence.cpp
             src/actionhelpers.cpp
             src/datasharingdlg.cpp
             src/remote/foepusher.cpp
)


set(HEADERS  include/foe_main.h
             include/foeclan.h
             include/foedatamanger.h
             include/foeuser.h
             include/foegoods.h
             include/foeusereditdlg.h
             include/foeages.h
             include/foeoverviewmodel.h
             include/foepersistence.h
             include/actionhelpers.h
             include/datasharingdlg.h
             src/remote/foepusher.h
)

set(FORMS ui/foe_main.ui
          ui/foeusereditdlg.ui
          ui/foe_clan.ui
          ui/datasharingdlg.ui
)


QT5_WRAP_UI(FOE_FORMS_HEADERS ${FORMS})

QT5_ADD_RESOURCES(FOE_RESOURCES resources/goods.qrc)

set(TRANSLATIONS translations/foe_da_DK.ts)
QT5_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS})

add_custom_target(lupdate
                  COMMAND lupdate ${SOURCES} ${HEADERS} ${FORMS} -Iinclude -ts ${TRANSLATIONS}
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_subdirectory( libpusher )

include_directories( include libpusher )

## Application libs
add_library(foeapp ${SOURCES} ${HEADERS} ${FOE_RESOURCES} ${RESOURCES} ${FOE_FORMS_HEADERS} ${QM_FILES})
target_link_libraries(foeapp Qt5::Core Qt5::Widgets Qt5::Network Qt5::Sql pusher)

## Main application
add_executable(FOE  src/main.cpp )
add_dependencies(FOE lupdate)
target_link_libraries(FOE Qt5::Core Qt5::Widgets Qt5::Network Qt5::Sql foeapp pusher)


# Copy libs
if (WIN32)
	message( "QT looks like to be installed in: " ${_qt5Core_install_prefix} )
	set(QT_DIST_DIR ${_qt5Core_install_prefix})
	add_custom_command(TARGET FOE POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/Qt5Core.dll                   ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/Qt5Widgets.dll                ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/Qt5Gui.dll                    ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/Qt5Sql.dll                    ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/Qt5Network.dll                ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/libgcc_s_dw2-1.dll            ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/libwinpthread-1.dll           ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/libstdc++-6.dll               ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/icudt53.dll                   ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/icuin53.dll                   ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/bin/icuuc53.dll                   ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/plugins/sqldrivers/qsqlite.dll    ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_DIST_DIR}/plugins/platforms/qwindows.dll    ${CMAKE_BINARY_DIR}
	)
endif()