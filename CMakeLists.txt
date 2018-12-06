cmake_minimum_required(VERSION 3.10)
project(nsd C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

set(SOURCE_FILES
        main.c
        inc/client_thread.h
        src/client_thread.c
        inc/logger.h
        src/logger.c
        libs/oscl/src/data.c
        libs/oscl/src/threads.c
        libs/oscl/src/time.c
        libs/oscl/src/utils.c
        libs/oscl/src/malloc.c

        libs/collections/src/lbq.c
        libs/collections/src/list.c
        libs/collections/src/map.c
        libs/collections/src/map2.c
        libs/collections/src/rings.c
        libs/oscl/include/data.h
        libs/oscl/include/threads.h
        libs/oscl/include/time.h
        libs/oscl/include/utils.h
        libs/oscl/include/malloc.h inc/cmd_processor.h src/cmd_processor.c)

add_executable(nsd ${SOURCE_FILES})

find_package(Threads)
target_link_libraries(nsd ${CMAKE_THREAD_LIBS_INIT})