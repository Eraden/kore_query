cmake_minimum_required(VERSION 2.8.11)
project(dummy)

SET(KORE_NO_TLS "TRUE")
SET(KORE_USE_PGSQL "TRUE")
SET(KORE_USE_TASKS "TRUE")
SET(_GNU_SOURCE "TRUE")
SET(TEST_KORE_QUERY "TRUE")

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
  execute_process(COMMAND psql postgres -f ${CMAKE_CURRENT_SOURCE_DIR}/test/create.psql)
  execute_process(COMMAND psql kore_query_test -f ${CMAKE_CURRENT_SOURCE_DIR}/test/setup.psql)
  execute_process(COMMAND psql kore_query_test -f ${CMAKE_CURRENT_SOURCE_DIR}/test/seed.psql)

  SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

  ENABLE_TESTING()

  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

  FIND_PACKAGE(Check REQUIRED)
  FIND_PACKAGE(Threads)
  FIND_PACKAGE(YAJL REQUIRED)
  FIND_PACKAGE(PostgreSQL REQUIRED)

  INCLUDE_DIRECTORIES(
    /usr/include
    /usr/local/include
    /usr/include/postgresql
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/kore/includes
  )

  INCLUDE_DIRECTORIES(/usr/include /usr/local/include /usr/include/postgresql)
  INCLUDE_DIRECTORIES(${CHECK_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${YAJL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${PostgreSQL_INCLUDE_DIRS})

  FILE(MAKE_DIRECTORY .includes)
  FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/kore/includes/
    DESTINATION .includes/kore)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/.includes)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/.includes/kore)

  MESSAGE(INFO " CHECK_INCLUDE_DIRS: ${CHECK_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS} ${YAJL_INCLUDE_DIR}")

  ADD_DEFINITIONS(-DUNICODE -D_UNICODE -D_MBCS)
  ADD_DEFINITIONS(-DLIBS=-ldl)

  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -ldl -coverage")
  IF (CMAKE_C_COMPILER MATCHES "clang")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  ELSE()
    MESSAGE(INFO " No memory tests will be performed!")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
  ENDIF()
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKORE_NO_TLS")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKORE_USE_PGSQL")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKORE_USE_TASKS")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST_KORE_QUERY")

  add_executable(
      test_kore_query

      ./mock/accesslog.c
      ./mock/config.c
      ./mock/domain.c
      ./mock/http.c
      ./mock/kore.c
      ./mock/pgsql.c

      #      ./vendor/kore/src/accesslog.c
      ./vendor/kore/src/auth.c
      #      ./vendor/kore/src/bsd.c
      ./vendor/kore/src/buf.c
      #      ./vendor/kore/src/cli.c
      #      ./vendor/kore/src/config.c
      ./vendor/kore/src/connection.c
      #      ./vendor/kore/src/domain.c
      #      ./vendor/kore/src/http.c
      #      ./vendor/kore/src/jsonrpc.c
      #      ./vendor/kore/src/keymgr.c
      #      ./vendor/kore/src/kore.c
      ./vendor/kore/src/linux.c
      ./vendor/kore/src/mem.c
      ./vendor/kore/src/module.c
      ./vendor/kore/src/msg.c
      ./vendor/kore/src/net.c
      #      ./vendor/kore/src/pgsql.c
      ./vendor/kore/src/pool.c
      ./vendor/kore/src/tasks.c
      ./vendor/kore/src/timer.c
      ./vendor/kore/src/utils.c
      ./vendor/kore/src/validator.c
      #      ./vendor/kore/src/websocket.c
      ./vendor/kore/src/worker.c

      ./json.c
      ./strings.c
      ./database_exec.c
      ./database_query.c
      ./database_query_stringify.c
      ./serialize.c

      test/test_strings.c
      test/test_json.c
      test/test_database_query.c
      test/test_database_query_stringify.c
      test/test_database_exec.c

      ./test/test.c
  )

  message(INFO " CHECK_LIBRARIES: ${CHECK_LIBRARIES}")
  message(INFO " CMAKE_THREAD_LIBS_INIT: ${CMAKE_THREAD_LIBS_INIT}")

  if(CMAKE_C_COMPILER MATCHES "gcc")
    include(CodeCoverage)
    setup_target_for_coverage(test_kore_query_coverage test_kore_query coverage)
  endif()

  target_link_libraries(
      test_kore_query
      ${CMAKE_THREAD_LIBS_INIT}
      ${CHECK_LIBRARIES}
      ${${YAJL_LIBRARY}}
      ${PostgreSQL_LIBRARIES}
      ${LIBS} -ldl
  )

  target_link_libraries(test_kore_query gcov)
ENDIF ()
