cmake_minimum_required(VERSION 3.26)

find_program(GLSLANG_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
if(NOT GLSLANG_VALIDATOR)
  message(FATAL_ERROR "glslangValidator not found!")
endif()

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SHADER_BINARY_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/content/shaders)
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

file(GLOB SHADER_SOURCE_FILES
	"${SHADER_SOURCE_DIR}/*.frag"
	"${SHADER_SOURCE_DIR}/*.vert"
	"${SHADER_SOURCE_DIR}/*.comp"
	"${SHADER_SOURCE_DIR}/*.geom"
	"${SHADER_SOURCE_DIR}/*.tesc"
	"${SHADER_SOURCE_DIR}/*.tese"
)

foreach(SHADER ${SHADER_SOURCE_FILES})
	get_filename_component(SHADER_NAME ${SHADER} NAME)
	set(SHADER_BINARY ${SHADER_BINARY_DIR}/${SHADER_NAME}.spv)
	add_custom_command(
	    OUTPUT ${SHADER_BINARY}
	    COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${SHADER_BINARY}
	    DEPENDS ${SHADER}
		VERBATIM
	)
	
	list(APPEND SHADER_BINARY_FILES ${SHADER_BINARY})
endforeach()

add_custom_target(
    LocusEngineShaders ALL
    DEPENDS ${SHADER_BINARY_FILES}
)