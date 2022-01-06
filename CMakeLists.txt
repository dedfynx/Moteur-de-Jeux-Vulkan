cmake_minimum_required(VERSION 3.20.0)

project("MoteurJeux" VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

######################################
#TODO
#Deplacer les dll avec l'executable
######################################

include_directories(
    ${CMAKE_SOURCE_DIR}/inc/ 
    ${CMAKE_SOURCE_DIR}/inc/GLFW/ 
    ${CMAKE_SOURCE_DIR}/lib/glm/ 
    ${CMAKE_SOURCE_DIR}/shaders/
    ${CMAKE_SOURCE_DIR}/lib/imgui/
    ${CMAKE_SOURCE_DIR}/lib/tinyObjLoader/
)

#if(WIN32)
#add_custom_target(
#                SHADERS
#                COMMAND cmd ${CMAKE_SOURCE_DIR}/shaders/compileShaders.bat
#                )
#
#endif()
#ADD_DEPENDENCIES("${PROJECT_NAME}" SHADERS)

#Liste des fichiers
file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/inc/*.h)
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB SHADERS_FILES ${CMAKE_SOURCE_DIR}/shaders/*.spv)
file(GLOB IMGUI_FILES ${CMAKE_SOURCE_DIR}/lib/imgui/*.cpp)


#Exec
add_executable("${PROJECT_NAME}" ${CMAKE_SOURCE_DIR} ${SRC_FILES} ${HEADER_FILES} ${IMGUI_FILES})

#Librairies
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan )

set(LIB_GLFW3 ${CMAKE_SOURCE_DIR}/lib/glfw3/lib/libglfw3dll.a)
target_link_libraries(${PROJECT_NAME} ${LIB_GLFW3})




