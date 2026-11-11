# Copyright(c) 2020 eaibot limited.
# Adapted for ROS 2 Jazzy & Ubuntu 24.04
cmake_minimum_required(VERSION 3.16)
project(ydlidar_ros2_driver C CXX)

################## ROS 2 / C++ Standard #############################
# ROS 2 Jazzy requires C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#################### find package ###################################
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_srvs REQUIRED)

############## YDLIDAR SDK ##########################################
find_package(ydlidar_sdk REQUIRED)

# Include directories
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${YDLIDAR_SDK_INCLUDE_DIRS})

# Link library directories
if(DEFINED YDLIDAR_SDK_LIBRARY_DIRS)
  link_directories(${YDLIDAR_SDK_LIBRARY_DIRS})
endif()

#---------------------------------------------------------------------------------------
# generate executable: ydlidar_ros2_driver_node
#---------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}_node
    src/${PROJECT_NAME}_node.cpp)

ament_target_dependencies(${PROJECT_NAME}_node
    "rclcpp"
    "rcutils"
    "sensor_msgs"
    "visualization_msgs"
    "geometry_msgs"
    "std_srvs"
)

if(TARGET ydlidar_sdk::ydlidar_sdk)
  target_link_libraries(${PROJECT_NAME}_node ydlidar_sdk::ydlidar_sdk)
else()
  target_link_libraries(${PROJECT_NAME}_node ${YDLIDAR_SDK_LIBRARIES})
endif()

#---------------------------------------------------------------------------------------
# generate executable: ydlidar_ros2_driver_client
#---------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}_client
    src/${PROJECT_NAME}_client.cpp)

ament_target_dependencies(${PROJECT_NAME}_client
    "rclcpp"
    "rcutils"
    "sensor_msgs"
    "visualization_msgs"
    "geometry_msgs"
    "std_srvs"
)

#---------------------------------------------------------------------------------------
# Install
#---------------------------------------------------------------------------------------
install(TARGETS
    ${PROJECT_NAME}_node ${PROJECT_NAME}_client
    DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch params startup config
    DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
endif()

ament_package()
