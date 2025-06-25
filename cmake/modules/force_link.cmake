# hopefully this works

function(fgla_force_link_backend target method lib)
    	if(NOT TARGET ${target})
        	message(FATAL_ERROR "Target '${target}' does not exist.")
    	endif()

	if(NOT TARGET ${lib})
		message(FATAL_ERROR "Target '${lib}' does not exist.")
	endif()

    	if(MSVC)
        	# Windows: /WHOLEARCHIVE:<libname>
        	get_target_property(target_output_name ${lib} OUTPUT_NAME)
        	if(NOT target_output_name)
        		set(target_output_name ${lib})
        	endif()
		target_link_options(${target} ${method} "/WHOLEARCHIVE:${target_output_name}.lib")
    	elseif(APPLE)
    	    	# macOS: -force_load <full-path>
    	    	get_target_property(target_location ${lib} LOCATION)
        	if(NOT target_location)
            		set_target_properties(${lib} PROPERTIES EXPORT_NAME ${lib})
            		# Fallback: force a build to get location
            		message(WARNING "Apple linker requires full .a path; make sure ${lib} is built before linking.")
        	endif()
        	target_link_libraries(${target} ${method} "-Wl,-force_load,$<TARGET_FILE:${lib}>")
    	else()
        	# Linux/Clang/GCC: --whole-archive
        	target_link_libraries(${target}
			${method}
			"-Wl,--whole-archive $<TARGET_FILE:${lib}> -Wl,--no-whole-archive"
		)
    	endif()
endfunction()

