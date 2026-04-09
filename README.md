# DG4F ROS 2

[![CI](https://github.com/tesollodelto/dg4f_ros2/actions/workflows/ci.yml/badge.svg)](https://github.com/tesollodelto/dg4f_ros2/actions/workflows/ci.yml)
![ROS 2 Humble](https://img.shields.io/badge/ROS_2-Humble-blue?logo=ros)
![ROS 2 Jazzy](https://img.shields.io/badge/ROS_2-Jazzy-blue?logo=ros)

ROS 2 packages for the **Delto Gripper DG4F** (4-finger).

## Packages

| Package | Description |
|---|---|
| `dg4f_description` | URDF/xacro model, meshes, and RViz display launch |
| `dg4f_driver` | ros2_control hardware driver and controller launch files |
| `dg4f_gz` | Gazebo simulation |
| `dg4f_moveit_config` | MoveIt 2 configuration (SRDF, planners, mock hardware) |

## Dependencies

This repository requires the following packages to build:

```bash
# Clone into your ROS 2 workspace src directory
git clone https://github.com/tesollodelto/dg_hardware.git
git clone https://github.com/tesollodelto/dg_tcp_comm.git
```

- [`delto_hardware`](https://github.com/tesollodelto/dg_hardware) — Unified hardware interface for Delto grippers
- [`delto_tcp_comm`](https://github.com/tesollodelto/dg_tcp_comm) — TCP communication library for Delto grippers

## Build

```bash
cd ~/ros2_ws
colcon build --packages-select dg4f_description dg4f_driver dg4f_gz dg4f_moveit_config
source install/setup.bash
```

## Launch

```bash
# RViz display
ros2 launch dg4f_description dg4f_display.launch.py

# Hardware driver
ros2 launch dg4f_driver dg4f_driver.launch.py

# Effort controller
ros2 launch dg4f_driver dg4f_effort_controller.launch.py

# Gazebo simulation
ros2 launch dg4f_gz dg4f_gz.launch.py

# Mock hardware (no device required)
ros2 launch dg4f_driver dg4f_mock.launch.py

# MoveIt (mock hardware, default)
ros2 launch dg4f_moveit_config dg4f_moveit.launch.py

# MoveIt (real hardware)
ros2 launch dg4f_moveit_config dg4f_moveit.launch.py use_mock:=false delto_ip:=169.254.186.72
```
