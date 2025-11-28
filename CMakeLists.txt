cmake_minimum_required(VERSION 3.10)

# 1. Name your project
project(PhysicsSimulation)

# 2. Set the C++ Standard (SFML 3 requires modern C++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Find the SFML library
# This command looks for SFML on the user's computer automatically.
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# 4. Create the Executable
# List all your .cpp files here (Header files .hpp don't need to be listed)
add_executable(Simulation main.cpp)

# 5. Link the libraries
# This tells the compiler to connect SFML to your specific executable.
target_link_libraries(Simulation PRIVATE SFML::Graphics SFML::Window SFML::System)

# ... (after target_link_libraries)

# Copy the font file to the build directory after compiling
add_custom_command(TARGET Simulation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/arial.ttf"
        $<TARGET_FILE_DIR:Simulation>
    COMMENT "Copying arial.ttf to the build directory..."
)