cmake_minimum_required(VERSION 3.5)

project(exchange-cli LANGUAGES C)

add_executable(exchange-cli main.c Node.h PriceLevel.h)
