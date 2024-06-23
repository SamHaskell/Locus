cmake_minimum_required(VERSION 3.26)
project(LocusEngine)

add_subdirectory(content/shaders)

###########################################################
###########################################################

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)
add_subdirectory(vendor/SDL2)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS)

add_subdirectory(vendor/yaml-cpp)

set(YAML_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/yaml-cpp/include
)

set(IMGUI_INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
)

set(IMGUI_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_sdl2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_vulkan.cpp
)

set(GLAD_INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

set(GLAD_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c
)

###########################################################
###########################################################

# C/CPP Source Files

set(BASE_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/Base/Logging.cpp
)

set(CORE_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Window.cpp
)

set(GRAPHICS_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/GraphicsDevice.cpp
)

set(MATH_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/Math/Numerics.cpp
)

set(PLATFORM_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Platform.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/LVK/LVKHelpers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/LVK/LVKImage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/LVK/LVKPipeline.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/LVK/LVKDescriptor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/LVK/LVKGraphicsDevice.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/LSDL/LSDLWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/LSDL/LSDLPlatform.cpp
)

###########################################################
###########################################################

set(SOURCE_FILES
	${BASE_SOURCE_FILES}
	${CORE_SOURCE_FILES}
	${GRAPHICS_SOURCE_FILES}
	${MATH_SOURCE_FILES}
	${PLATFORM_SOURCE_FILES}
	
	${IMGUI_SOURCE_FILES}
	${GLAD_SOURCE_FILES}
)

add_library(LocusEngine SHARED ${SOURCE_FILES})
add_dependencies(LocusEngine LocusEngineShaders)

target_link_libraries(LocusEngine 
	PUBLIC 
		SDL2::SDL2
		yaml-cpp::yaml-cpp
		Vulkan::Vulkan
)

target_include_directories(LocusEngine 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${SDL_INCLUDE_DIR}
		${Vulkan_INCLUDE_DIRS}
		${YAML_INCLUDE_DIRS}
		${GLAD_INCLUDE_DIRS}
		${IMGUI_INCLUDE_DIRS}
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}/include
)