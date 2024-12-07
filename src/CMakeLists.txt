

set(SOURCES
  main.cpp
)

find_package(GTest CONFIG REQUIRED)


# Add executable
add_executable(engine ${SOURCES})

# Compile / migrate shaders
include("assets/shaders/compile_shaders.cmake")

# Make a script in the assets folder to migrate shaders, textures, and models. aswell as anything else we need.
# It should also include a universal folder that will be copied directly for misc items

# Migrate textures
include("assets/textures/migrate_textures.cmake")

# Migrate models
include("assets/models/migrate_models.cmake")

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(io)
add_subdirectory(logger)
add_subdirectory(window_manager)

target_link_libraries(engine
  PRIVATE
    io
    logger
    window_manager
)

# Unit testing
include(testing.cmake)






