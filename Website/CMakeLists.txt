find_package(ZLIB 1.2.3 REQUIRED) # defines imported target ZLIB::ZLIB
message(STATUS "zstr - found ZLIB (version: ${ZLIB_VERSION_STRING})")

file(GLOB_RECURSE Sources Source/*.cpp)
include_directories(Include)
add_executable(website ${Sources})
set_property(TARGET website PROPERTY C_STANDARD 23)
target_link_libraries(website PRIVATE link ZLIB::ZLIB mysqlcppconn /usr/local/lib/libvmime.so.1.0.0 curl)