project(FOEClanManager)
cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0020 NEW)

## Config
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

message("Buil type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_COMPILER_IS_GNUCXX)
##-fvisibility=hidden -fvisibility-inlines-hidden
	set(CMAKE_CXX_FLAGS                "-pipe -Wextra -Wall -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG          "-Og -ggdb")
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
set(SOURCES  src/main.cpp
             src/foe_main.cpp
             src/foeclan.cpp
             src/foedatamanger.cpp
             src/foeuser.cpp
             src/foegoods.cpp
             src/foeusereditdlg.cpp
             src/foeages.cpp
             src/foeoverviewmodel.cpp
             src/foepersistence.cpp
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
)

set(FORMS ui/foe_main.ui
          ui/foeusereditdlg.ui
          ui/foe_clan.ui
)


QT5_WRAP_UI(FOE_FORMS_HEADERS ${FORMS})

QT5_ADD_RESOURCES(FOE_RESOURCES resources/goods.qrc)

set(TRANSLATIONS translations/foe_da_DK.ts)
QT5_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS})

add_custom_target(lupdate
                  COMMAND lupdate ${SOURCES} ${HEADERS} ${FORMS} -ts ${TRANSLATIONS}
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_subdirectory( libpusher )

include_directories( include libpusher )
add_executable(FOE  ${SOURCES} ${HEADERS} ${FOE_RESOURCES} ${RESOURCES} ${FOE_FORMS_HEADERS} ${QM_FILES} )
add_dependencies(FOE lupdate)

# Use the Core module from Qt 5.
target_link_libraries(FOE Qt5::Core Qt5::Widgets Qt5::Network Qt5::Sql pusher)
