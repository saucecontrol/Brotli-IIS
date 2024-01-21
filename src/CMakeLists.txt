cmake_minimum_required(VERSION 3.20)

set(BROTLI_VERSION_MAJOR 1)
set(BROTLI_VERSION_MINOR 1)
set(BROTLI_VERSION_PATCH 0)
set(BROTLI_VERSION_REVIS 0)
set(BROTLI_VERSION_PARCH " (${VCPKG_TARGET_ARCHITECTURE})")
set(BROTLI_LINK_LIBS brotlienc brotlicommon)

string(TIMESTAMP DATE_YEAR "%Y")

project(brotli C)

find_package(Brotli NAMES unofficial-brotli REQUIRED)
foreach(brlib IN LISTS BROTLI_LINK_LIBS)
  add_library(${brlib} ALIAS unofficial::brotli::${brlib})
endforeach()

configure_file(brotli.rc.in brotli.rc)

add_library(brotli brotli.c brotli.def brotli.rc)

target_link_libraries(brotli PRIVATE ${BROTLI_LINK_LIBS})

install(TARGETS brotli RUNTIME DESTINATION bin)
