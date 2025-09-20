cmake_minimum_required(VERSION 4.0.2)
project(Project1)

# 헤더 파일이 있는 디렉토리를 추가합니다.
#include_directories(include)

# 소스 파일이 있는 디렉토리와 파일들을 지정합니다.
add_executable(Project1
        Invader.c
        Console.c
        Mychar.c
        Juck.c
)