cmake_minimum_required(VERSION 3.21)
project(kpiadx.kpi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# MSVC では、KbMedia Player 本体付属のプラグインと同じく VC++ ランタイムを動的リンクする (既定の /MD)。
# MinGW (開発中の確認用) では libstdc++ などの DLL に依存しないよう静的リンクする

add_library(kpiadx SHARED
  kpiadx.cpp
  kpiadx.def
  kpiadx.h
  KpiADXDecoder.cpp
  KpiADXDecoder.h
  KpiADXDecoderModule.cpp
  KpiADXDecoderModule.h
  KpiPartialFile.h
  aax.h
  adx.h
  bitstream.h
  kpi_mingw_uuid.h
  stdafx.h
)
target_include_directories(kpiadx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kpisdk)
target_compile_definitions(kpiadx PRIVATE UNICODE _UNICODE)   # NOMINMAX は stdafx.h で定義している
target_link_libraries(kpiadx PRIVATE ws2_32)   # ntohs / ntohl

if(MSVC)
  target_compile_options(kpiadx PRIVATE /utf-8 /W3)
else()
  # SDK ヘッダの __declspec(uuid()) は kpi_mingw_uuid.h で補うので、gcc の警告は抑える
  target_compile_options(kpiadx PRIVATE -Wall $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>
    $<$<CXX_COMPILER_ID:Clang>:-fms-extensions>)
  target_link_options(kpiadx PRIVATE -static)
endif()

set_target_properties(kpiadx PROPERTIES
  OUTPUT_NAME kpiadx
  PREFIX ""
  SUFFIX ".kpi")

# 検証用ホストとテスト
option(KPIADX_BUILD_TOOLS "Build kpi-render and the tests" ON)
# 手元の ADX/AAX/AFS を置いたディレクトリ。指定すると、その中のファイルでも seek-check を行う
set(KPIADX_TEST_DATA "" CACHE PATH "Directory of ADX/AAX/AFS files for seek-check")

if(KPIADX_BUILD_TOOLS)
  add_executable(kpi-render tools/kpi-render.cpp)
  target_include_directories(kpi-render PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kpisdk)
  target_compile_definitions(kpi-render PRIVATE UNICODE _UNICODE NOMINMAX)
  if(MSVC)
    target_compile_options(kpi-render PRIVATE /utf-8 /W3)
  else()
    target_compile_options(kpi-render PRIVATE -Wall $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>
      $<$<CXX_COMPILER_ID:Clang>:-fms-extensions>)
    target_link_options(kpi-render PRIVATE -municode -static)
  endif()
  # kpi-render は同じディレクトリの kpiadx.kpi を使うので、出力先をそろえる
  set_target_properties(kpiadx kpi-render PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  add_dependencies(kpi-render kpiadx)

  # クロスビルドでは、CMAKE_CROSSCOMPILING_EMULATOR (wine など) があるときだけテストを登録する
  enable_testing()
  find_package(Python3 COMPONENTS Interpreter)
  if(NOT CMAKE_CROSSCOMPILING OR CMAKE_CROSSCOMPILING_EMULATOR)
    if(Python3_FOUND)
      # 合成した ADX/AAX/AFS を、tools/fixtures.py の参照デコーダーの結果と照合する
      set(fixtures_dir ${CMAKE_BINARY_DIR}/fixtures)
      set(emulator_args)
      if(CMAKE_CROSSCOMPILING_EMULATOR)
        set(emulator_args --emulator ${CMAKE_CROSSCOMPILING_EMULATOR})
      endif()
      add_test(NAME make-fixtures
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/fixtures.py generate ${fixtures_dir})
      set_tests_properties(make-fixtures PROPERTIES FIXTURES_SETUP fixtures)
      add_test(NAME check-fixtures
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/fixtures.py check ${fixtures_dir}
          --cli $<TARGET_FILE:kpi-render> ${emulator_args})
      set_tests_properties(check-fixtures PROPERTIES FIXTURES_REQUIRED fixtures)
      add_test(NAME seek-check-fixtures
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/fixtures.py seek-check ${fixtures_dir}
          --cli $<TARGET_FILE:kpi-render> ${emulator_args})
      set_tests_properties(seek-check-fixtures PROPERTIES FIXTURES_REQUIRED fixtures)
      # ファイルが多いとコマンドラインの長さの上限を超えるので、fixtures.py が分けて kpi-render に渡す
      if(KPIADX_TEST_DATA)
        add_test(NAME seek-check-data
          COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/fixtures.py seek-check-data
            ${KPIADX_TEST_DATA} --cli $<TARGET_FILE:kpi-render> ${emulator_args})
      endif()
    endif()
  endif()
endif()
