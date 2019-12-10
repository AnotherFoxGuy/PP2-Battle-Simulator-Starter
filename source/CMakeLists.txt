project(PP2)

include(cotire)

####################################################################################################
#  SOURCE FILES
####################################################################################################

set(SOURCE_FILES
        ThreadPool.h
        prerequisites.h
        explosion.{h,cpp}
        game.{h,cpp}
        particle_beam.{h,cpp}
        rocket.{h,cpp}
        smoke.{h,cpp}
        surface.{h,cpp}
        tank.{h,cpp}
        template.{h,cpp}
        )


include(SourceFileUtils)

# Expand file extensions (i.e. path/to/file.{h,cpp} becomes path/to/file.h;path/to/file.cpp)
expand_file_extensions(SOURCE_FILES ${SOURCE_FILES})

# Generate source groups for use in IDEs
generate_source_groups(${SOURCE_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL CONAN_PKG::sdl2 CONAN_PKG::freeimage)

cotire(${PROJECT_NAME})

add_custom_target(
        copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/
        COMMENT "Copy the resources to the compiled directory"
        VERBATIM
)

