cmake_minimum_required (VERSION 3.7.1)

set(COMPONENTS uv)

macro(_uv_ADJUST_LIB_VARS basename)
	
	if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set(uv_${basename}_LIBRARY optimized ${uv_${basename}_LIBRARY_RELEASE} debug ${uv_${basename}_LIBRARY_DEBUG})
    else()
        # if there are no configuration types and CMAKE_BUILD_TYPE has no value
        # then just use the release libraries
        set(uv_${basename}_LIBRARY ${uv_${basename}_LIBRARY_DEBUG} )
    endif()
      # FIXME: This probably should be set for both cases
    set(uv_${basename}_LIBRARIES optimized ${uv_${basename}_LIBRARY_RELEASE} debug ${uv_${basename}_LIBRARY_DEBUG})
	
endmacro()

set(UV_DIR "uv-ROOT-NOTFOUND" CACHE PATH "Path to the root uv directory" )
set(uv_LIBRARYDIR "uv-LIBRARY-NOTFOUND" CACHE PATH "Path to the static uv Library" )


if (${UV_DIR} EQUAL "uv-ROOT-NOTFOUND")
	message( FATAL_ERROR "Variable UV_DIR is empty")
endif()

set(uv_INCLUDEDIR "${UV_DIR}/include;" CACHE PATH "include path for uv" FORCE)

if (WIN32)
  set(uv_LIBRARYDIR "${UV_DIR}/lib" CACHE PATH "include path for uv" FORCE)
endif()

if(UNIX)
  set(uv_LIBRARYDIR "${UV_DIR}/lib" CACHE PATH "include path for uv" FORCE)
endif()



set(lib_path ${uv_LIBRARYDIR})
set(_lib_list "")


if (WIN32)
set(EXTENSION .lib)
else()
set(EXTENSION .so)
endif()

foreach( COMPONENT  ${COMPONENTS} )
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
	if (NOT TARGET uv::${COMPONENT})
		string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		set(uv_${UPPERCOMPONENT}_LIBRARY "")
		set(uv_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
		set(uv_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
	endif()
endforeach()

foreach( COMPONENT  ${COMPONENTS} )
	if (NOT TARGET uv::${COMPONENT})
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		message(STATUS "Set LIBRARY : ${COMPONENT}")
		set(uv_${UPPERCOMPONENT}_LIBRARY "")
		if (WIN32)
		  set(uv_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/debug/lib${COMPONENT}${EXTENSION}")
		  set(uv_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/debug/lib${COMPONENT}${EXTENSION}")
		endif()

		if(UNIX)
		  set(uv_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/debug/lib${COMPONENT}${EXTENSION}")
		  set(uv_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/debug/lib${COMPONENT}${EXTENSION}")
		endif()
		
		_uv_ADJUST_LIB_VARS(${UPPERCOMPONENT})
		add_library(uv::${COMPONENT} SHARED IMPORTED)
		message(STATUS "Select LIBRARY : ${uv_${UPPERCOMPONENT}_LIBRARY}")
		
		if(EXISTS "${uv_${UPPERCOMPONENT}_LIBRARY}")
			  set_target_properties(uv::${COMPONENT} PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
				IMPORTED_LOCATION "${uv_${UPPERCOMPONENT}_LIBRARY}")
		endif()
		if(EXISTS "${uv_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		  set_property(TARGET uv::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS RELEASE)
		  set_target_properties(uv::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
			IMPORTED_LOCATION_RELEASE "${uv_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		endif()
		if(EXISTS "${uv_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		  set_property(TARGET uv::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS DEBUG)
		  set_target_properties(uv::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
			IMPORTED_LOCATION_DEBUG "${uv_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		endif()
	endif()
endforeach()

set(uv_LIBRARIES ""  CACHE LIST "uvMedia libraries " FORCE)

 foreach( COMPONENT  ${COMPONENTS} )
    string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
   
     list(APPEND uv_LIBRARIES ${uv_${UPPERCOMPONENT}_LIBRARY})
    
  endforeach()
  ##Append last dll comming from vendor and zlib
list(APPEND uv_LIBRARIES ${uv_EXTERNAL_LIBRARY})
  
 message(STATUS "uv LIBRARIES : ${uv_LIBRARIES}")
