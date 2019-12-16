project(PP2)


####################################################################################################
#  SOURCE FILES
####################################################################################################

set(SOURCE_FILES
        ThreadPool.h
        explosion.{h,cpp}
        game.{h,cpp}
        particle_beam.{h,cpp}
        rocket.{h,cpp}
        smoke.{h,cpp}
        surface.{h,cpp}
        tank.{h,cpp}
        template.{h,cpp}
        prerequisites.h
        )


include(SourceFileUtils)

# Expand file extensions (i.e. path/to/file.{h,cpp} becomes path/to/file.h;path/to/file.cpp)
expand_file_extensions(SOURCE_FILES ${SOURCE_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC .)
if (USE_PACKAGE_MANAGER)
    # Generate source groups for use in IDEs
    generate_source_groups(${SOURCE_FILES})

    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
    #target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
    target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::sdl2)
    target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::freeimage)

else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

    target_include_directories(${PROJECT_NAME} PRIVATE ${FreeImage_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${FREEIMAGE_LIBRARY})
endif ()

if (USE_PACKAGE_MANAGER)
    include(cotire)
    cotire(${PROJECT_NAME})
endif ()
add_custom_target(
        copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/
        COMMENT "Copy the resources to the compiled directory"
        VERBATIM
)

