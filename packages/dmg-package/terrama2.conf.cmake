# Force set of variables:
set (CMAKE_INSTALL_PREFIX "$ENV{TM_INSTALL}/" CACHE PATH "Where to install package?" FORCE)
set (CMAKE_PREFIX_PATH "/usr;$ENV{TL_THIRD_DIR};$ENV{TM_THIRD_DIR}" CACHE PATH "Where to find libraries?" FORCE)
set (terralib_DIR "$ENV{TL_PATH}" CACHE PATH "Where to find TerraLib?" FORCE)

# Turn off/on TerraMa2 CMake variables:
set (TERRAMA2_BUILD_EXAMPLES_ENABLED OFF CACHE BOOL "Build Examples?" FORCE)
set (TERRAMA2_BUILD_UNITTEST_ENABLED OFF CACHE BOOL "Build Unittest?" FORCE)
set (TERRAMA2_BUILD_AS_BUNDLE ON CACHE BOOL "Build As Bundle?" FORCE)
set (TERRAMA2_TRACK_3RDPARTY_DEPENDENCIES ON CACHE BOOL "Build Track 3rdparty?" FORCE)

# Set the package type to be generated:
set (CPACK_BINARY_DRAGNDROP ON CACHE BOOL "Use DMG?" FORCE)
set (CPACK_BINARY_PACKAGEMAKER OFF CACHE BOOL "Use PackageMaker for package?" FORCE)
set (CPACK_BINARY_BUNDLE OFF CACHE BOOL "Use Bundle for package?" FORCE)
set (CPACK_BINARY_7Z OFF CACHE BOOL "Use 7zip for package?" FORCE)
set (CPACK_BINARY_IFW OFF CACHE BOOL "Use IFW for package?" FORCE)
set (CPACK_BINARY_NSIS OFF CACHE BOOL "Use NSIS for package?" FORCE)
set (CPACK_BINARY_WIX OFF CACHE BOOL "Use Wix for package?" FORCE)
set (CPACK_BINARY_ZIP OFF CACHE BOOL "Use Zip for package?" FORCE)
set (CPACK_BINARY_STGZ OFF CACHE BOOL "Use STGZ for package?" FORCE)
set (CPACK_BINARY_TGZ OFF CACHE BOOL "Use TGZ for package?" FORCE)
set (CPACK_SOURCE_TBZ2 OFF CACHE BOOL "Use TBZ2 for package?" FORCE)
set (CPACK_SOURCE_TXZ OFF CACHE BOOL "Use TXZ for package?" FORCE)
set (CPACK_SOURCE_TZ OFF CACHE BOOL "Use TZ for package?" FORCE)
set (CPACK_SOURCE_RPM OFF CACHE BOOL "Use RPM for package?" FORCE)
