# Force set of install variables:
set (CMAKE_INSTALL_PREFIX "$ENV{TM_INSTALL}/" CACHE PATH "Where to install package?" FORCE)
set (CMAKE_INSTALL_RPATH "$ENV{TM_INSTALL}/lib" CACHE PATH "Where to install package libs?" FORCE)
set (CMAKE_PREFIX_PATH "$ENV{DEPENDENCIES};$ENV{TL_PATH}/3rdparty" CACHE PATH "Where to find libraries?" FORCE)

set (terralib_DIR "$ENV{TL_PATH}/lib/cmake/terralib-5.4.5" CACHE PATH "Where to find TerraLib?" FORCE)

# Config RPath variables:
set (CMAKE_SKIP_BUILD_RPATH OFF CACHE BOOL "Skip build rpath?" FORCE)
set (CMAKE_BUILD_WITH_INSTALL_RPATH OFF CACHE BOOL "Build with install rpath?" FORCE)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH ON CACHE BOOL "Rpath use link path?" FORCE)

# Turn off TerraLib CMake variables:
set (TERRAMA2_BUILD_AS_BUNDLE OFF CACHE BOOL "Build As Bundle?" FORCE)
set (TERRAMA2_TRACK_3RDPARTY_DEPENDENCIES OFF CACHE BOOL "Build Track 3rdparty?" FORCE)
set (TERRAMA2_BUILD_EXAMPLES_ENABLED OFF CACHE BOOL "Build Examples?" FORCE)
set (TERRAMA2_BUILD_UNITTEST_ENABLED OFF CACHE BOOL "Build Unittest?" FORCE)

# Set the package type to be generated:
set (CPACK_BINARY_DEB ON CACHE BOOL "Generate DEB?" FORCE)
set (CPACK_BINARY_STGZ OFF CACHE BOOL "Generate STGZ?" FORCE)
set (CPACK_BINARY_TGZ OFF CACHE BOOL "Generate TGZ?" FORCE)
set (CPACK_BINARY_TZ OFF CACHE BOOL "Generate TZ?" FORCE)
set (CPACK_SOURCE_TBZ2 OFF CACHE BOOL "Generate TBZ2 Source?" FORCE)
set (CPACK_SOURCE_TGZ OFF CACHE BOOL "Generate TGZ  Source?" FORCE)
set (CPACK_SOURCE_TZ OFF CACHE BOOL "Generate TZ Source?" FORCE)
