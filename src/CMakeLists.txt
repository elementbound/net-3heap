cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project("Server" C)

set(COMMON_SOURCES	"game.c"
					"protocol.c"
					"net.c"
					"stringbuffer.c")

add_executable("server" ${COMMON_SOURCES})

if(CMAKE_COMPILER_IS_GNUCC) 
	set(CMAKE_C_FLAGS "-std=c99")
else()
	message(WARNING "Unknown compiler. You might want to set your C standard to C99")
endif()