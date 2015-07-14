include_directories("inc")
include_directories("${CLR_CMAKE_CORECLR_PRODUCT_DIR}/inc")

if(CLR_CMAKE_PLATFORM_UNIX)
  # This prevents inclusion of standard C compiler headers
  add_compile_options(-nostdinc)

  if(NOT CLR_CMAKE_PLATFORM_DARWIN)
    # This prevents inclusion of standard C++ compiler headers
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++")
  endif(NOT CLR_CMAKE_PLATFORM_DARWIN)

  include_directories("${CLR_CMAKE_CORECLR_DIR}/src/pal/inc")
  include_directories("${CLR_CMAKE_CORECLR_DIR}/src/pal/inc/rt")
  include_directories("${CLR_CMAKE_CORECLR_DIR}/src/inc")  
endif(CLR_CMAKE_PLATFORM_UNIX)

add_subdirectory(inc)
add_subdirectory(profiler)
