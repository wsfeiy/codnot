cmake_minimum_required(VERSION 3.30.0)

# 输出编译命令到 compile_commands.json 文件，方便 IDE 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 打印详细的构建过程信息，便于调试
set(CMAKE_VERBOSE_MAKEFILE ON)

set(BUILD_SHARED_LIBS ON)

# 设置二进制目录
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/cmake/build/${CMAKE_BUILD_TYPE})

# ------------------------------------------------------------------------------
# 项目设置
# ------------------------------------------------------------------------------

project(codnot VERSION 0.5.0 LANGUAGES C CXX HOMEPAGE_URL "https://github.com/wsfeiy/codnot")

# 设置 C 和 C++ 编译标准，分别为 C17 和 C++20，并且要求严格遵循标准
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.c)

file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/codnot/*.h ${CMAKE_SOURCE_DIR}/include/codnot/*.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE CODNOT_BUILD)

# 设置库的私有包含路径
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/)

# 设置 CMake 文件夹路径
set(CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)

# 包含 GenerateExportHeader 模块
include(GenerateExportHeader)

# 为库生成导出头文件
generate_export_header(${PROJECT_NAME})

# 设置库的版本和 SO 版本
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 3)

# 设置接口属性，用于版本控制
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_CODNOT_MAJOR_VERSION ${PROJECT_VERSION})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING INTERFACE_CODNOT_MAJOR_VERSION)

# ------------------------------------------------------------------------------
# 根据操作系统进行特定设置
# ------------------------------------------------------------------------------

if(UNIX AND NOT APPLE)
 # 针对Linux发行版
 set(CMAKE_INSTALL_PREFIX "/packages/c/codnot/")
 target_compile_definitions(${PROJECT_NAME} PRIVATE __linux__)
 # 添加标准库路径
 include_directories(/usr/include/c++/12)
 include_directories(/usr/include/x86_64-linux-gnu/c++/12)
 include_directories(/usr/lib/gcc/x86_64-linux-gnu/12/include)
elseif(APPLE)
 # 针对macOS
 target_compile_definitions(${PROJECT_NAME} PRIVATE __MACOS__)
elseif(WIN32)
 # 针对Windows
 message("building by win...")
 target_compile_definitions(${PROJECT_NAME} PRIVATE __WIN__)
 set(CMAKE_INSTALL_PREFIX "E:/packages/c/codnot/")
endif()

# ------------------------------------------------------------------------------
# 根据不同预设进行特定设置
# -----------------------------------------------------------------------------

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Release 配置下的设置
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
else()
    # 其他配置下的默认设置
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

# ------------------------------------------------------------------------------
# 函数：setup_package
# 描述：构建Cmake包
# ------------------------------------------------------------------------------

function(setup_package)

# 设置包配置文件安装路径
set(ConfigPackageLocation lib/cmake/codnot)

# 设置主配置文件路径
set(PKG_MAIN_CONFIG "${CMAKE_DIR}/codnot/${PROJECT_NAME}Config.cmake")

# 设置版本配置文件路径
set(PKG_VERSION_CONFIG "${CMAKE_DIR}/codnot/${PROJECT_NAME}ConfigVersion.cmake")

# 设置目标文件路径
set(PKG_TARGET_CONFIG "${CMAKE_DIR}/codnot/${PROJECT_NAME}Targets.cmake")

# 安装库文件和头文件
install(TARGETS ${PROJECT_NAME} EXPORT codnotTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# 安装头文件到 include 目录下
install(
    DIRECTORY
      include/codnot
    DESTINATION
      include
    COMPONENT
    Devel
)

# 包含 CMakePackageConfigHelpers 模块
include(CMakePackageConfigHelpers)

# 写入版本配置文件
write_basic_package_version_file(
  "${PKG_VERSION_CONFIG}"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# 导出目标文件
export(EXPORT codnotTargets
  FILE "${PKG_TARGET_CONFIG}"
  NAMESPACE ${PROJECT_NAME}::
)

# 配置主配置文件
configure_file(
  "${CMAKE_DIR}/codnot/codnotConfig.cmake.in"
  "${PKG_MAIN_CONFIG}"
  @ONLY
)

# 安装导出的目标文件
install(EXPORT codnotTargets
  FILE
  codnotTargets.cmake
  NAMESPACE
  ${PROJECT_NAME}::
  DESTINATION
    ${ConfigPackageLocation}
)

# 安装配置文件
install(
  FILES
    "${PKG_MAIN_CONFIG}"
    "${PKG_VERSION_CONFIG}"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

endfunction()

# 只在 Release 模式下调用 setup_package 函数
if(CMAKE_BUILD_TYPE STREQUAL "Release")
message(STATUS "Setting up package configuration")
    setup_package()
endif()

add_executable(Test test/main.cpp)

target_include_directories(Test PRIVATE include)

target_link_libraries(Test PRIVATE ${PROJECT_NAME})


include(CTest)
enable_testing()
add_test(NAME Test COMMAND Test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
