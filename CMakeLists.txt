cmake_minimum_required(VERSION 3.31.6)
project(Project1)

# utf-8 컴파일 옵션 적용
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>)

# 헤더 파일이 있는 디렉토리 추가
include_directories(include)

# 소스 파일이 있는 디렉토리와 파일 지정
add_executable(Project1
    Invader.c
    src/Console.c
    src/MyChar.c
    src/Enemy.c
    src/Util.c
)

# 생성된 실행 파일에 라이브러리 연결
target_link_libraries(Project1 winmm)

# 프로젝트 루트 경로(/Project1)를 나타내는 매크로 정의
target_compile_definitions(Project1
    PRIVATE # 소스 파일에만 적용
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)