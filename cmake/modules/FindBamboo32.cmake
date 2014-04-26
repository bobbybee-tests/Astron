# Locate Bamboo32
#
# This module defines
#  BAMBOO32_FOUND, if false, do not try to link to bamboo
#  BAMBOO32_LIBRARIES, where to find bamboo object files
#  BAMBOO32_INCLUDE_DIRS, where to find bamboo headers
#  BAMBOO32_LIBRARY_DIRS, the directory containing BAMBOO32_LIBRARIES
#
# By default, the dynamic libraries of bamboo will be found. To find the static ones instead,
# you must set the BAMBOO_USE_STATIC_LIBS variable to TRUE before calling find_package(Bamboo ...).

# attempt to find static library first if this is set
if(BAMBOO32_USE_STATIC_LIBS)
    set(BAMBOO32_STATIC bamboo32.a)
endif()

# find the bamboo include directory
find_path(BAMBOO32_INCLUDE_DIRS bamboo/module/Module.h
          PATH_SUFFIXES include
          PATHS
          ~/Library/Frameworks/bamboo/include/
          /Library/Frameworks/bamboo/include/
          /usr/local/include/
          /usr/include/
          /opt/bamboo/
          ${PROJECT_SOURCE_DIR}/dependencies/bamboo/include)

# find the bamboo library
find_library(BAMBOO32_LIBRARIES
             NAMES ${BAMBOO32_STATIC} bamboo bamboo.lib
             PATH_SUFFIXES lib64 lib
             PATHS ~/Library/Frameworks
                    /Library/Frameworks
                    /usr/local
                    /usr
                    /sw
                    /opt/local
                    /opt/csw
                    /opt
                    ${PROJECT_SOURCE_DIR}/dependencies/bamboo/)

get_filename_component(BAMBOO32_LIBRARY_DIRS ${BAMBOO32_LIBRARIES} PATH)

# handle the QUIETLY and REQUIRED arguments and set BAMBOO_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bamboo32 DEFAULT_MSG BAMBOO32_INCLUDE_DIRS BAMBOO32_LIBRARIES BAMBOO32_LIBRARY_DIRS)
mark_as_advanced(BAMBOO32_INCLUDE_DIRS BAMBOO32_LIBRARY_DIRS BAMBOO32_LIBRARIES)
