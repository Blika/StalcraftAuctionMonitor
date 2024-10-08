cmake_minimum_required(VERSION 3.11.0)
if(UNIX)
    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -s")
else(WIN32)
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc.exe)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++.exe)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -s")
    set(BOOST_ROOT "c:/boost_1_82_0")
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib/)
endif()
set(CMAKE_CXX_STANDARD 20)
if(DEBUG)
    add_compile_options(-g)
    project(debug__stalcraft_auction_monitor)
else()
    project(stalcraft_auction_monitor)
endif()
if(UNIX)
    set(Boost_USE_STATIC_LIBS OFF) 
    set(Boost_USE_MULTITHREADED OFF)  
    set(Boost_USE_STATIC_RUNTIME OFF) 
    find_package(Boost 1.71.0 COMPONENTS locale) 
    include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src)
else(WIN32)
    include_directories(${BOOST_ROOT} ${PROJECT_SOURCE_DIR}/src)
endif()
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
add_definitions(-DCURL_STATICLIB)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} curl ${BOOST_LIBRARYDIR}libboost_locale-mgw13-mt-s-x64-1_82.a)
else(UNIX)
    target_link_libraries(${PROJECT_NAME} curl ${Boost_LIBRARIES})
endif()