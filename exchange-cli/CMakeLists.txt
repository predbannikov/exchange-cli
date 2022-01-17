cmake_minimum_required(VERSION 3.5)

project(exchange-cli LANGUAGES C)

#set(CMAKE_C_FLAGS "-Wall -Wextra")

#set(CMAKE_C_FLAGS_RELEASE "-O2")

add_executable(exchange-cli main.c Node.h PriceLevel.h hashtable.h oidstore.h )
