# Locate Bamboo
#
# This module defines
#  BAMBOO_FOUND, if false, do not try to link to bamboo
#  BAMBOO_LIBRARIES, where to find bamboo object files
#  BAMBOO_INCLUDE_DIRS, where to find bamboo headers
#  BAMBOO_LIBRARY_DIRS, the directory containing BAMBOO_LIBRARIES
#
# By default, the dynamic libraries of bamboo will be found. To find the static ones instead,
# you must set the BAMBOO_USE_STATIC_LIBS variable to TRUE before calling find_package(Bamboo ...).

# attempt to find static library first if this is set
if(BAMBOO_USE_STATIC_LIBS)
    set(BAMBOO_STATIC bamboo.a)
endif()

# find the bamboo include directory
find_path(BAMBOO_INCLUDE_DIRS bamboo/module/Module.h
          PATH_SUFFIXES include
          PATHS
          ~/Library/Frameworks/bamboo/include/
          /Library/Frameworks/bamboo/include/
          /usr/local/include/
          /usr/include/
          /opt/bamboo/
          ${PROJECT_SOURCE_DIR}/dependencies/bamboo/include)

# find the bamboo library
find_library(BAMBOO_LIBRARIES
             NAMES ${BAMBOO_STATIC} bamboo bamboo.lib
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

get_filename_component(BAMBOO_LIBRARY_DIRS ${BAMBOO_LIBRARIES} PATH)

# handle the QUIETLY and REQUIRED arguments and set BAMBOO_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bamboo DEFAULT_MSG BAMBOO_INCLUDE_DIRS BAMBOO_LIBRARIES BAMBOO_LIBRARY_DIRS)
mark_as_advanced(BAMBOO_INCLUDE_DIRS BAMBOO_LIBRARY_DIRS BAMBOO_LIBRARIES)
