file(GLOB_RECURSE Sources Source/*.cpp)
include_directories(Include)
add_executable(website ${Sources})
target_link_libraries(website PRIVATE link)