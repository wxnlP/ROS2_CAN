cmake_minimum_required(VERSION 3.8)
project(can_bridge_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ros2_socketcan REQUIRED)
find_package(ros2_socketcan_msgs REQUIRED)

add_executable(can_sender_cpp src/can_sender.cpp)
add_executable(can_receiver_cpp src/can_receiver.cpp)

ament_target_dependencies(
  can_sender_cpp
  "rclcpp"
  "ros2_socketcan"
  "ros2_socketcan_msgs"
)
ament_target_dependencies(
  can_receiver_cpp
  "rclcpp"
  "ros2_socketcan"
  "ros2_socketcan_msgs"
)

install(TARGETS
  can_sender_cpp
  can_receiver_cpp
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
