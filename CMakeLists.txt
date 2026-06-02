cmake_minimum_required(VERSION 3.10)
project(physical_key_monitor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 编译选项
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O2 -Wall")

# AddressSanitizer 支持
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
if(ENABLE_ASAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

# 找到所需的库
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVDEV libevdev)
pkg_check_modules(SQLITE3 sqlite3)
pkg_check_modules(LIBEVENT libevent)

# 包含目录
include_directories(${PROJECT_SOURCE_DIR}/src)

# 源文件列表
set(SOURCES
    src/main.c
    src/monitor.c
    src/config.c
    src/utils/logger.c
    src/utils/queue.c
    src/utils/string_utils.c
    src/core/input_device.c
    src/core/event_listener.c
    src/processor/event_processor.c
    src/processor/anomaly_detector.c
    src/processor/state_tracker.c
    src/storage/database.c
    src/storage/event_store.c
    src/analysis/statistics.c
    src/api/http_server.c
    src/api/rest_api.c
)

# 可执行文件
add_executable(physical-key-monitor ${SOURCES})

# 链接库
target_link_libraries(physical-key-monitor m pthread)

if(LIBEVDEV_FOUND)
    target_link_libraries(physical-key-monitor ${LIBEVDEV_LIBRARIES})
    include_directories(${LIBEVDEV_INCLUDE_DIRS})
endif()

if(SQLITE3_FOUND)
    target_link_libraries(physical-key-monitor ${SQLITE3_LIBRARIES})
    include_directories(${SQLITE3_INCLUDE_DIRS})
endif()

if(LIBEVENT_FOUND)
    target_link_libraries(physical-key-monitor ${LIBEVENT_LIBRARIES})
    include_directories(${LIBEVENT_INCLUDE_DIRS})
endif()

# 输出目录
set_target_properties(physical-key-monitor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 安装目标
install(TARGETS physical-key-monitor DESTINATION bin)
install(FILES config.yaml DESTINATION etc/physical-key-monitor)
