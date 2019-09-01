
cmake_minimum_required (VERSION 3.5)
project(downloader)

include_directories("./include")

# debug
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# add source file
set(${PROJECT_NAME}_sources
    main.c
    common.c
    http_response.c
    http_request.c
    manager.c
    task.c
)

# executable
add_executable(main ${${PROJECT_NAME}_sources})

# link library
set(${PROJECT_NAME}_libraries
    pthread
)

target_link_libraries(main ${${PROJECT_NAME}_libraries})

