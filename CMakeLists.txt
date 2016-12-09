cmake_minimum_required (VERSION 2.8.11)
project (dummy)

add_executable(
    database_exec.c
    ./json.c
    ./strings.c
    ./database_query.c
    ./.main.c
)
