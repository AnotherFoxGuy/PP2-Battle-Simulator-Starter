cmake_minimum_required(VERSION 3.14)
project("PP2 Battle Simulator")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(FeatureSummary)
include(pmm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_USE_RELATIVE_PATHS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


pmm(CONAN REMOTES OGRE-dependencies https://api.bintray.com/conan/anotherfoxguy/OGRE-dependencies BINCRAFTERS
        CMakeCM ROLLING
        )

find_package(OpenGL REQUIRED)

# AVX2 support (Intel Haswell and higher)
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-mavx2")

add_subdirectory(source)

feature_summary(WHAT ALL)