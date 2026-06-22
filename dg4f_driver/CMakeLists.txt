cmake_minimum_required(VERSION 3.12)
project(dg4f_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(delto_hardware REQUIRED)

add_executable(dg4f_operator_driver src/dg4f_operator_driver.cpp)
target_link_libraries(dg4f_operator_driver
  rclcpp::rclcpp
  ${trajectory_msgs_TARGETS}
  ${sensor_msgs_TARGETS}
  ${std_msgs_TARGETS}
)

target_include_directories(dg4f_operator_driver PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS 
  dg4f_operator_driver
  DESTINATION lib/${PROJECT_NAME})

install(
  PROGRAMS 
  script/dg4f_pid_test.py
  script/dg4f_pid_all_test.py
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY config urdf launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
