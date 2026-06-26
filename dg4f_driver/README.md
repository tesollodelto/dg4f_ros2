# dg4f_driver ROS 2 Package 🚀

## 📌 Overview

The `dg4f_driver` ROS 2 package provides a hardware interface leveraging [ros2_control](https://control.ros.org/) for the DG-4F grippers (18 DOF: 4 fingers × 4 joints + 2 inner joints), enabling direct robotic control operations.

## 📦 Dependency Installation

### Navigate to Workspace
```bash
cd ~/your_ws
```

### Update rosdep
```bash
sudo apt update
rosdep update
```

### Install Specific Dependencies
```bash
rosdep install --from-paths src/tesollo_ros2/dg4f_ros2/dg4f_driver --ignore-src -r -y
```

### Verify Installation by Building
```bash
colcon build --packages-select dg4f_driver delto_hardware
```

---

## 🚀 Launch Files

| Launch File | Description | Controller Type |
|-------------|-------------|-----------------|
| `dg4f_driver.launch.py` | DG4F - JointTrajectoryController | Position (Trajectory) |
| `dg4f_pid_controller.launch.py` | DG4F - Individual per-joint PID Controllers | PID (Position→Effort) |
| `dg4f_pid_all_controller.launch.py` | DG4F - Single grouped PID Controller (all joints) | PID (Position→Effort) |
| `dg4f_effort_controller.launch.py` | DG4F - Direct Effort Control | Effort (Direct) |

---

## 🎛️ Controlling Delto Gripper-4F

### 1. Loading DG4F controller

Launch the Delto Gripper-4F controller with:
```bash
ros2 launch dg4f_driver dg4f_driver.launch.py delto_ip:=169.254.186.72 delto_port:=502
```

### 2. Loading DG4F PID controllers

Two PID variants are available (both: position reference → effort output):

```bash
# Individual: one PidController per joint (topic: /dg4f/<joint>_pospid/reference)
ros2 launch dg4f_driver dg4f_pid_controller.launch.py delto_ip:=169.254.186.72

# All-in-one: a single grouped PidController for every joint (topic: /dg4f/j_dg_pospid/reference)
ros2 launch dg4f_driver dg4f_pid_all_controller.launch.py delto_ip:=169.254.186.72
```

### 3. Loading DG4F Effort controller

For direct effort control:
```bash
ros2 launch dg4f_driver dg4f_effort_controller.launch.py delto_ip:=169.254.186.72
```

### 4. Test scripts:

| Script | Controller Type | Description |
|--------|-----------------|-------------|
| `dg4f_pid_test.py` | PID (individual) | Publishes a reference to each per-joint `*_pospid` controller |
| `dg4f_pid_all_test.py` | PID (all) | Publishes one reference to the grouped `j_dg_pospid` controller |

```bash
# Individual per-joint PID
ros2 run dg4f_driver dg4f_pid_test.py

# Single grouped PID (all joints)
ros2 run dg4f_driver dg4f_pid_all_test.py
```

---

## 🔧 Controller Types

### 1. JointTrajectoryController (Default)
- **Purpose**: Smooth trajectory interpolation for position control
- **Joints**: 18 joints
  - Fingers: j_dg_1_1~1_4, j_dg_2_1~2_4, j_dg_3_1~3_4, j_dg_4_1~4_4
  - Inner: j_dg_1_inner, j_dg_4_inner
- **Topic**: `/dg4f/delto_controller/joint_trajectory`

### 2. PID Controllers (Position → Effort)

Naming convention (consistent across all Delto drivers):

| Variant | Config | Controllers | Reference Topic |
|---------|--------|-------------|-----------------|
| **Individual** (`pid`) | `dg4f_pid_controller.yaml` | one `pid_controller/PidController` per joint, named `<joint>_pospid` | `/dg4f/<joint>_pospid/reference` |
| **All-in-one** (`pid_all`) | `dg4f_pid_all_controller.yaml` | a single `pid_controller/PidController` named `j_dg_pospid` managing all joints | `/dg4f/j_dg_pospid/reference` |

Both take a `control_msgs/MultiDOFCommand` position reference and output effort. Gains are seeded from the JTC config (`p: 1.5`).

### 3. Effort Controller (Direct)
- **Purpose**: Direct effort control without position feedback
- **Use Case**: Direct force control, impedance control
- **Topic**: `/dg4f/effort_controller/commands`

---

## 🌐 Namespace

All DG4F drivers use the `/dg4f/` namespace to avoid topic conflicts with other grippers.

---

## 🤝 Contributing
Contributions are encouraged:

1. Fork repository
2. Create branch (`git checkout -b feature/my-feature`)
3. Commit changes (`git commit -am 'Add my feature'`)
4. Push (`git push origin feature/my-feature`)
5. Open pull request

## 📄 License
BSD-3-Clause

## 📧 Contact
[TESOLLO SUPPORT](mailto:support@tesollo.com)
