cmake_minimum_required (VERSION 3.9)

project ("LeavingHome")

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable("${CMAKE_PROJECT_NAME}" ${sources})

add_custom_target(copy-resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res/ ${CMAKE_BINARY_DIR}/res/
    DEPENDS "${CMAKE_PROJECT_NAME}")

find_package(OpenMP)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)
add_subdirectory(lib/entt)
add_subdirectory(lib/tinyobjloader)

include_directories(
	lib/glfw/include/
	lib/glew/include/
	lib/glm/glm/
	lib/imgui/
	lib/stb/
	.
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	-lGL -lGLU -lXrandr -lXext -lX11 -lrt
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set(GLEW_SOURCE
	lib/glew/src/glew.c
)

set(GLEW_HEADERS
)

add_library( glew STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
)

target_link_libraries(glew
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

add_library(imgui STATIC
	"lib/imgui/imgui.cpp"	
	"lib/imgui/imgui_demo.cpp"
	"lib/imgui/imgui_draw.cpp"
	"lib/imgui/imgui_widgets.cpp"
	"lib/imgui/imgui_impl_opengl3.cpp"
	"lib/imgui/imgui_impl_glfw.cpp"
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	glew
	glm
	imgui
	EnTT
	OpenMP::OpenMP_CXX
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

target_link_libraries("${CMAKE_PROJECT_NAME}" ${ALL_LIBS})

