cmake_minimum_required(VERSION 3.16)
project(tower_defense_sfml CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 3 「静态」编译进 exe（免拷 DLL）。SFML 本体 + 它的文字依赖（freetype/harfbuzz/sheenbidi）
# 都已随仓库入库（SFML-3.1.0/ 与 deps/）；下面让 FetchContent 一律用这些本地源码，
# 【完全不联网、不 clone 任何东西】—— 锁定版本、离线可构建。
include(FetchContent)
set(FETCHCONTENT_SOURCE_DIR_SFML      "${CMAKE_SOURCE_DIR}/SFML-3.1.0"       CACHE PATH "用仓库自带的 SFML 源码，不联网"      FORCE)
set(FETCHCONTENT_SOURCE_DIR_FREETYPE  "${CMAKE_SOURCE_DIR}/deps/freetype-src"  CACHE PATH "用仓库自带的 freetype，不联网"  FORCE)
set(FETCHCONTENT_SOURCE_DIR_HARFBUZZ  "${CMAKE_SOURCE_DIR}/deps/harfbuzz-src"  CACHE PATH "用仓库自带的 harfbuzz，不联网"  FORCE)
set(FETCHCONTENT_SOURCE_DIR_SHEENBIDI "${CMAKE_SOURCE_DIR}/deps/sheenbidi-src" CACHE PATH "用仓库自带的 sheenbidi，不联网" FORCE)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 3.0.0
  GIT_SHALLOW ON
)
set(BUILD_SHARED_LIBS OFF)                        # 静态链接，免拷 DLL
set(SFML_BUILD_AUDIO   OFF CACHE BOOL "" FORCE)   # M0 用不到音频，先关，编译更快（要做声音再打开）
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

# 用仓库自带的 freetype（deps/ 下）时，SFML 给 freetype 开的 unity-build 会按「build/_deps 路径」
# 排除某些不能合并的源文件——但我们的源码在 deps/ 下、路径对不上，排除失效 → 合并编译时宏冲突。
# 关掉 freetype 的 unity-build 即可（只是稍慢一点，freetype 很小无所谓）。
if(TARGET freetype)
    set_target_properties(freetype PROPERTIES UNITY_BUILD OFF)
endif()

add_executable(ark main.cpp)
target_link_libraries(ark PRIVATE SFML::Graphics SFML::Window SFML::System)
set_target_properties(ark PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")   # ark.exe 生成到项目根目录
