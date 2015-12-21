set(CMAKE_INCLUDE_CURRENT_DIR ON)

# WINTODO: Conditionalize the next check
# AMD64 specific sources subdirectory
set(AMD64_SOURCES_DIR amd64)

# Needed due to the cmunged files being in the binary folders, the set(CMAKE_INCLUDE_CURRENT_DIR ON) is not enough
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}) 

add_definitions(-D_TARGET_AMD64_=1)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

if(CLR_CMAKE_PLATFORM_UNIX)
    add_compile_options(-fPIC)
endif(CLR_CMAKE_PLATFORM_UNIX)

set(SOURCES
    classfactory.cpp
	dllmain.cpp
	profiler.cpp
)

if(WIN32)
    set(SOURCES_AMD64_ASM
		${AMD64_SOURCES_DIR}/asmhelpers.asm    
	)
else()
	set(SOURCES_AMD64_ASM
		${AMD64_SOURCES_DIR}/unixasmhelpers.S    
	)
endif(WIN32)

list(APPEND SOURCES
    ${SOURCES_AMD64_ASM}    
)

if (WIN32)
	#Preprocess exports definition file
	preprocess_def_file(${CMAKE_CURRENT_SOURCE_DIR}/profiler.def ${CMAKE_CURRENT_BINARY_DIR}/profiler.def)
	list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/profiler.def)
endif(WIN32)
  
add_library(profiler-test
	SHARED
	${SOURCES}
)

if(CLR_CMAKE_PLATFORM_UNIX)
	find_library(CLR_CMAKE_CLR_CORGUIDS_LIB 
		corguids
		HINTS 
		"${CLR_CMAKE_CORECLR_PRODUCT_DIR}/lib"
	)
	find_library(CLR_CMAKE_CLR_CORECLRPAL_LIB 
		coreclrpal
		HINTS 
		"${CLR_CMAKE_CORECLR_PRODUCT_DIR}/lib"
	)			
	find_library(CLR_CMAKE_CLR_PALRT_LIB 
		palrt
		HINTS 
		"${CLR_CMAKE_CORECLR_PRODUCT_DIR}/lib"
	)	
	set(CLR_CMAKE_CLR_LIBS ${CLR_CMAKE_CLR_CORECLRPAL_LIB} ${CLR_CMAKE_CLR_PALRT_LIB} ${CLR_CMAKE_CLR_CORGUIDS_LIB} dl pthread) 
else()
	# find_library will pick from Windows Kits\8.1
	set(CLR_CMAKE_CLR_CORGUIDS_LIB "${CLR_CMAKE_CORECLR_PRODUCT_DIR}/lib/corguids.lib")
	set(CLR_CMAKE_CLR_LIBS ${CLR_CMAKE_CLR_CORGUIDS_LIB}) 
endif(CLR_CMAKE_PLATFORM_UNIX)

install (TARGETS profiler-test DESTINATION .)
set_target_properties(profiler-test PROPERTIES PREFIX "")

target_link_libraries(profiler-test 
	${CLR_CMAKE_CLR_LIBS}
)
