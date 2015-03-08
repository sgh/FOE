project(libpusher)
cmake_minimum_required(VERSION 2.8)

## Config
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
##-fvisibility=hidden -fvisibility-inlines-hidden
	set(CMAKE_CXX_FLAGS                "-flto -Wextra -Wall -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG          "-Og -g -ggdb")
	set(CMAKE_CXX_FLAGS_MINSIZEREL     "-s -Os -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE        "-s -O2 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-s -O2 -g")
endif()
if (NOT BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS on)
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
set(SOURCES  main.cpp
             foe_main.cpp
             foeclan.cpp
             foedatamanger.cpp
             foeuser.cpp
             foegoods.cpp
             foeusereditdlg.cpp
             foeages.cpp
             foeoverviewmodel.cpp
             foeconnectiondetails.cpp
)


set(HEADERS  foe_main.h
             foeclan.h
             foedatamanger.h
             foeuser.h
             foegoods.h
             foeusereditdlg.h
             foeages.h
             foeoverviewmodel.h
             foeconnectiondetails.h
)

set(FORMS foe_main.ui
          foeusereditdlg.ui
          foeconnectiondetails.ui
          foe_clan.ui
)


QT5_WRAP_UI(FOE_FORMS_HEADERS ${FORMS})

QT5_ADD_RESOURCES(FOE_RESOURCES goods.qrc)

set(TRANSLATIONS foe_da_DK.ts)
QT5_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS})


add_executable(FOE  ${SOURCES} ${HEADERS} ${FOE_RESOURCES} ${RESOURCES} ${FOE_FORMS_HEADERS} ${QM_FILES} )

# Use the Core module from Qt 5.
target_link_libraries(FOE Qt5::Core Qt5::Widgets Qt5::Network Qt5::Sql)


# TRANSLATIONS = "foe_da_DK.ts"
#

#
#
# lupdate.commands = lupdate FOE.pro
# lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
#
# lrelease.commands = lrelease FOE.pro
# lrelease.depends = lupdate
