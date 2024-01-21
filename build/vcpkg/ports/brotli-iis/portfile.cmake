set(VCPKG_POLICY_DLLS_WITHOUT_LIBS enabled)

set(REPO_ROOT "${CURRENT_PORT_DIR}/../../../..")
set(SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src)

file(GLOB BROTLI_SOURCES ${REPO_ROOT}/src/*)
file(COPY ${BROTLI_SOURCES} DESTINATION ${SOURCE_PATH})

vcpkg_cmake_configure(SOURCE_PATH ${SOURCE_PATH})
vcpkg_cmake_install()
vcpkg_copy_pdbs()

write_file(${CURRENT_PACKAGES_DIR}/include/${PORT}.h "//dummy header")
vcpkg_install_copyright(FILE_LIST "${REPO_ROOT}/license")
