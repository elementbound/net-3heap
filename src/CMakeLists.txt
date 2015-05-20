cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project("net-3heaps" C)

set(COMMON_SOURCES	"game.c"
					"protocol.c"
					"net.c"
					"stringbuffer.c")

add_executable("server" "server.c" ${COMMON_SOURCES})
add_executable("client" "client.c" ${COMMON_SOURCES})

if(CMAKE_COMPILER_IS_GNUCC) 
	set(CMAKE_C_FLAGS "-std=c99")
	set(CMAKE_C_FLAGS_DEBUG "-ggdb")
else()
	message(WARNING "Unknown compiler. You might want to set your C standard to C99")
endif()

target_link_libraries("server" "ws2_32")
target_link_libraries("client" "ws2_32")