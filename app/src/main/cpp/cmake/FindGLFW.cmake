# 尝试使用 CMake 的配置模式查找 GLFW
if(NOT GLFW_FOUND)
    find_path(GLFW_INCLUDE_DIRS glfw3.h
            HINTS
            ../BlackPearl/vendor/GLFW/include
            PATH_SUFFIXES glfw
    )

    find_library(GLFW_LIBRARIES NAMES glfw3 glfw
            HINTS
            ../BlackPearl/vendor/GLFW/bin
    )
#
#    if(GLFW_INCLUDE_DIRS AND GLFW_LIBRARIES)
#        set(GLFW_FOUND TRUE)
#        message(STATUS "Found GLFW: ${GLFW_LIBRARIES} (include: ${GLFW_INCLUDE_DIRS})")
#    endif()

    # 如果需要，可以设置其他相关变量，如版本检查
    # if(GLFW_FOUND AND NOT TARGET GLFW::GLFW)
    #     add_library(GLFW::GLFW UNKNOWN IMPORTED)
    #     set_target_properties(GLFW::GLFW PROPERTIES
    #         IMPORTED_LOCATION ${GLFW_LIBRARIES}
    #         INTERFACE_INCLUDE_DIRECTORIES ${GLFW_INCLUDE_DIRS}
    #     )
    # endif()
endif()

# 如果没有找到，可以在这里设置 NOTFOUND 消息
if(NOT GLFW_FOUND)
    message(FATAL_ERROR "Could not find GLFW library")
endif()