include(GetPrerequisites)

macro(hipe_find_dependencies list_deps target_name BUILD_CONFIG)
  if (WIN32)

	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/bin/${BUILD_CONFIG}/${target_name}${EXT_BIN}" cm_path)
	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/bin/${BUILD_CONFIG}" cm_dir)

  else()

	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/lib/${BUILD_CONFIG}/lib${target_name}${EXT_BIN}" cm_path)
	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/lib/${BUILD_CONFIG}" cm_dir)

  endif()
	if (WIN32)
		set(PATH_SHAREDLIB "${HIPE_EXTERNAL_DIR}/boost_1_62_0/lib64-msvc-14.0;${HIPE_EXTERNAL_DIR}/intel64_win;${HIPE_EXTERNAL_DIR}/caffe/bin;${HIPE_EXTERNAL_DIR}/gflags/bin/${BUILD_CONFIG};${HIPE_EXTERNAL_DIR}/glog/bin/;${HIPE_EXTERNAL_DIR}/opencv-3.4/x64/vc14/bin;${HIPE_EXTERNAL_DIR}/gstreamer/1.0/x86_64/bin;${cm_dir};${CMAKE_PREFIX_PATH};${CMAKE_INSTALL_PREFIX}/bin/${BUILD_CONFIG};${CMAKE_INSTALL_PREFIX}/lib/${BUILD_CONFIG}")
	else()
		set(PATH_SHAREDLIB "${HIPE_EXTERNAL_DIR}/boost/lib;${HIPE_EXTERNAL_DIR}/opencv/lib;${cm_dir};${CMAKE_PREFIX_PATH};${HIPE_EXTERNAL_DIR}/caffe/lib;${CMAKE_INSTALL_PREFIX}/bin/${BUILD_CONFIG};${HIPE_EXTERNAL_DIR}/dlib/lib/;${CMAKE_INSTALL_PREFIX}/lib/${BUILD_CONFIG}")
	message(STATUS "SEARCH PATH : ${PATH_SHAREDLIB}")
	endif()
	message(STATUS "Info file path [ ${cm_path} ] in dir [ ${cm_dir} ] ")
	get_prerequisites("${cm_path}" PREREQS 1 1 "" "${PATH_SHAREDLIB};${Hipecore_DIR}/bin/${BUILD_CONFIG}")
	
	message(STATUS "prerequisites ${PREREQS} for ${cm_path}")
	foreach(DEPENDENCY_FILE ${PREREQS})
	gp_resolve_item("${cm_path}" "${DEPENDENCY_FILE}" "" "${PATH_SHAREDLIB};${Hipecore_DIR}/bin/${BUILD_CONFIG}" resolved_file)
	if (UNIX)
		get_filename_component( dep_realpath "${resolved_file}" REALPATH )
		get_filename_component( dep_name "${resolved_file}" NAME )
		LIST(APPEND list_deps "${dep_realpath}")

	endif()
	message(STATUS "resolved_file='${resolved_file}'")
	LIST(APPEND list_deps "${resolved_file}")
	endforeach()
endmacro()
