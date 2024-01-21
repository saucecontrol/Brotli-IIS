set(VCPKG_POLICY_DLLS_WITHOUT_LIBS enabled)
set(VCPKG_POLICY_EMPTY_INCLUDE_FOLDER enabled)

set(REPO_ROOT "${CURRENT_PORT_DIR}/../../../..")
set(SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src)

file(GLOB BROTLI_SOURCES ${REPO_ROOT}/src/*)
file(COPY ${BROTLI_SOURCES} DESTINATION ${SOURCE_PATH})

vcpkg_cmake_configure(SOURCE_PATH ${SOURCE_PATH})
vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_install_copyright(FILE_LIST "${REPO_ROOT}/license")
