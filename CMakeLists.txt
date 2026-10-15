# https://Github/cortl0/MaxDoF
# GPL-3.0 license

cmake_minimum_required( VERSION 3.5.1 )
project( MaxDoF )

set(MAXDOF_PATH_DEPENDENCIES "${CMAKE_CURRENT_SOURCE_DIR}/../MaxDoF-Dependencies")

set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" )

include( CMake/Bootstrap.cmake )

include( CMake/Dependencies/OGRE.cmake )

if( APPLE )
	set( CMAKE_CXX_STANDARD 11 )
endif()

set(OGRE_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/ogre-next" CACHE STRING "Path to OGRE-Next source code")
set(OGRE_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/ogre-next/build/Debug" CACHE STRING "Path to OGRE-Next build folder")

setupOgre( OGRE_SOURCE, OGRE_BINARIES, OGRE_LIBRARIES, FALSE, FALSE, FALSE )

find_package(SDL2 REQUIRED)

set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" )
if( MSVC )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	if( NOT PLATFORM_X64 )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
	endif()
	add_definitions( -DUNICODE -D_UNICODE )
endif()

if( APPLE )
	macro( add_recursive dir retVal )
		file( GLOB_RECURSE ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c ${dir}/*.mm ${dir}/*.m )
	endmacro()
else()
	macro( add_recursive dir retVal )
		file( GLOB_RECURSE ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c )
	endmacro()
endif()

include_directories( "./include" )
# Ogre doesn't need this include, but we do because of Rapidjson in UnitTesting.cpp
include_directories( "./Dependencies/Ogre/Dependencies/include" )

include_directories("${MAXDOF_PATH_DEPENDENCIES}/jolt-physics")
link_directories("${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build")
add_subdirectory("${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build" "${CMAKE_CURRENT_BINARY_DIR}/JoltBuild")

add_recursive( ./src SOURCES )
add_recursive( ./include HEADERS )

if( APPLE )
	if( APPLE_IOS )
		file( GLOB_RECURSE RESOURCES ./src/*.storyboard )
	endif()
	set( RESOURCES ${RESOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/bin/Data )

	if( APPLE_IOS )
		set_source_files_properties(
			${CMAKE_CURRENT_SOURCE_DIR}/src/OgreCommon/System/OSX/OSXUtils.mm
			PROPERTIES HEADER_FILE_ONLY TRUE
		)
	else()
		set_source_files_properties(
			${CMAKE_CURRENT_SOURCE_DIR}/src/OgreCommon/System/iOS/AppDelegate.mm
			${CMAKE_CURRENT_SOURCE_DIR}/src/OgreCommon/System/iOS/GameViewController.mm
			${CMAKE_CURRENT_SOURCE_DIR}/src/OgreCommon/System/iOS/iOSUtils.mm
			PROPERTIES HEADER_FILE_ONLY TRUE
		)
	endif()
endif()

if( APPLE )
	# Treat our *.c & *.cpp files as *.mm + Enable ARC on them
	set_source_files_properties( ${SOURCES} PROPERTIES
		COMPILE_FLAGS
			"-x objective-c++ -fobjc-arc"
	)
endif()

add_executable( ${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${RESOURCES} )
target_compile_definitions( ${PROJECT_NAME} PRIVATE MAXDOF_CLIENT_BUILD)
target_link_libraries( ${PROJECT_NAME} PRIVATE Jolt ${OGRE_LIBRARIES} ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

if( APPLE )
	set_target_properties( ${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO" )
	set_target_properties( ${PROJECT_NAME} PROPERTIES RESOURCE "${RESOURCES}" )
	#set_target_properties( ${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE SampleBrowser_OSX.icns)
	set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
		-framework CoreGraphics -framework QuartzCore -framework Metal  -framework StoreKit" )

	if( APPLE_IOS )
		# iPhone
		set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
			-framework UIKit -framework CoreText -framework CoreMotion" )

		# These must be global for all projects; or else they'll cause
		# trouble or increased compiling times
		set( CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "10.0" )
		set( CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "arm64" )

		set_target_properties( ${PROJECT_NAME} PROPERTIES
			MACOSX_BUNDLE_INFO_PLIST
				${CMAKE_CURRENT_SOURCE_DIR}/Scripts/Apple/iPhone/iOSApp.plist
			# XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS
			#	"${CMAKE_CURRENT_SOURCE_DIR}/Scripts/Apple/iPhone/iOSEntitlements.plist"
		)
	else()
		# macOS
		set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Foundation" )
		set_target_properties( ${PROJECT_NAME} PROPERTIES
			MACOSX_BUNDLE_INFO_PLIST
				${CMAKE_CURRENT_SOURCE_DIR}/Scripts/Apple/macOS/macOSApp.plist
			# XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS
			#	"${CMAKE_CURRENT_SOURCE_DIR}/Scripts/Apple/macOS/macOSEntitlements.plist"
		)
	endif()
endif()
