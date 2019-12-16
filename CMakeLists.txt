cmake_minimum_required(VERSION 3.12)
project("PP2 Battle Simulator")

option(USE_PACKAGE_MANAGER "Use conan for managing packages" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(FeatureSummary)


# AVX2 support (Intel Haswell and higher)
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-mavx2")

if (USE_PACKAGE_MANAGER)
    include(pmm)
    pmm(CONAN REMOTES OGRE-dependencies https://api.bintray.com/conan/anotherfoxguy/OGRE-dependencies BINCRAFTERS
            CMakeCM ROLLING
            )
else ()
    find_package(SDL2 REQUIRED)
    find_package(FreeImage REQUIRED)
endif ()

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_USE_RELATIVE_PATHS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


add_subdirectory(source)

feature_summary(WHAT ALL)
