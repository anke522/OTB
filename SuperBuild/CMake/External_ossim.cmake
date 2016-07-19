INCLUDE_ONCE_MACRO(OSSIM)

SETUP_SUPERBUILD(OSSIM)

# declare dependencies
ADDTO_DEPENDENCIES_IF_NOT_SYSTEM(OSSIM TIFF GEOTIFF GEOS JPEG OPENTHREADS FREETYPE)

ADD_SUPERBUILD_CMAKE_VAR(OSSIM TIFF_INCLUDE_DIR)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM TIFF_LIBRARY)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM GEOTIFF_INCLUDE_DIR)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM GEOTIFF_LIBRARY)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM GEOS_INCLUDE_DIR)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM GEOS_LIBRARY)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM JPEG_INCLUDE_DIR)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM JPEG_LIBRARY)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM OPENTHREADS_INCLUDE_DIR)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM OPENTHREADS_LIBRARY)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM FREETYPE_INCLUDE_DIRS)
ADD_SUPERBUILD_CMAKE_VAR(OSSIM FREETYPE_LIBRARY)

set(OSSIM_CXX_FLAGS  -D__STDC_CONSTANT_MACROS)

if(MSVC)
  set(OSSIM_CXX_FLAGS /EHsc)
endif()

# archive version
ExternalProject_Add(OSSIM
  PREFIX OSSIM
  URL "http://download.osgeo.org/ossim/source/ossim-1.8.20/ossim-1.8.20-3.tar.gz"
  URL_MD5 eb2265db0d4d9201e255b92317121cfd
  BINARY_DIR ${OSSIM_SB_BUILD_DIR}
  INSTALL_DIR ${SB_INSTALL_PREFIX}
  DOWNLOAD_DIR ${DOWNLOAD_LOCATION}
  CMAKE_CACHE_ARGS
  ${SB_CMAKE_CACHE_ARGS}
  -DProject_WC_REVISION:STRING=23665
  -DCMAKE_CXX_FLAGS:STRING=${OSSIM_CXX_FLAGS}
  -DBUILD_OSSIM_MPI_SUPPORT:BOOL=OFF
  -DBUILD_OSSIM_FREETYPE_SUPPORT:BOOL=ON
  -DBUILD_OSSIM_APPS:BOOL=OFF
  -DBUILD_OSSIM_TESTS:BOOL=OFF
  -DBUILD_OSSIM_TEST_APPS:BOOL=OFF
  -DBUILD_OSSIM_FRAMEWORKS:BOOL=OFF
  -DINSTALL_ARCHIVE_DIR:STRING=lib
  -DINSTALL_LIBRARY_DIR:STRING=lib
  ${OSSIM_SB_CONFIG}
  DEPENDS ${OSSIM_DEPENDENCIES}
  CMAKE_COMMAND ${SB_CMAKE_COMMAND}
  )

ExternalProject_Add_Step(OSSIM patch_no_cmakelists
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_SOURCE_DIR}/patches/OSSIM/CMakeLists.txt
  ${OSSIM_SB_SRC}
  DEPENDEES patch
  DEPENDERS configure )

SUPERBUILD_PATCH_SOURCE(OSSIM)

SUPERBUILD_UPDATE_CMAKE_VARIABLES(OSSIM FALSE)
