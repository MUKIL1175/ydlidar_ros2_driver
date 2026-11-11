![YDLIDAR](images/YDLidar.jpg "YDLIDAR")
# YDLIDAR ROS 2 Driver (ROS 2 Jazzy & Ubuntu 24.04 Compatible)

`ydlidar_ros2_driver` is the ROS 2 driver package for YDLIDAR devices. This version is fully adapted and tested for **ROS 2 Jazzy Jalisco on Ubuntu 24.04 LTS (Noble Numbat)**, while retaining full compatibility with **ROS 2 Humble**.

---

## What's New in this ROS 2 Jazzy & Ubuntu 24.04 Edition

1. **C++17 & GCC 14 Compliance**:
   - `CMakeLists.txt` upgraded to C++17 (`CMAKE_CXX_STANDARD 17` required) and CMake 3.16+ minimum version.
   - Clean compilation under GCC 14 on Ubuntu 24.04 without deprecated warnings or missing standard headers (`<cmath>`, `<limits>`, `<rcutils/time.h>`).

2. **ROS 2 Jazzy Node & Launch Refactoring**:
   - Replaced invalid `LifecycleNode` declarations with standard `launch_ros.actions.Node` for clean startup in Jazzy.
   - Updated `tf2_ros static_transform_publisher` arguments to modern flag arguments (`--x`, `--y`, `--z`, `--qx`, `--qy`, `--qz`, `--qw`, `--frame-id`, `--child-frame-id`).
   - Replaced deprecated `launch/ydlidar.py` script with modern ROS 2 launch format.
   - Added dedicated launch files for **YDLidar Tmini / Tmini Plus**:
     - `tmini_plus_launch.py`: Starts driver using `params/Tmini.yaml`.
     - `tmini_plus_view.py`: Starts driver and opens RViz2 with pre-configured Best Effort QoS.

3. **RViz2 Best Effort QoS Match**:
   - Configured `config/ydlidar.rviz` to `Reliability Policy: Best Effort` so laser scans are immediately visible in RViz2 without QoS mismatch errors.

4. **Parameter System & Nav2 Optimization**:
   - Parameter declaration updated to avoid `InvalidParameterTypeException` with floating-point YAML parameters.
   - Supported `invalid_range_is_inf`: correctly converts invalid/out-of-range scan readings to infinity (`inf`), as expected by Nav2.
   - Resilient zero-timestamp handling: gracefully falls back to `node->get_clock()->now()` if hardware timestamp is unset.

5. **Ubuntu 24.04 udev Support**:
   - `startup/initenv.sh` updated to use `udevadm control --reload-rules && udevadm trigger` for modern systemd.

---

## 1. Prerequisites (Ubuntu 24.04 & ROS 2 Jazzy)

Make sure you have installed ROS 2 Jazzy desktop tools and build dependencies:
```bash
sudo apt update
sudo apt install -y build-essential cmake git pkg-config python3-dev python3-colcon-common-extensions \
    ros-jazzy-rclcpp ros-jazzy-sensor-msgs ros-jazzy-geometry-msgs ros-jazzy-visualization-msgs \
    ros-jazzy-std-srvs ros-jazzy-tf2-ros ros-jazzy-rviz2
```

---

## 2. Build & Install YDLidar-SDK

`ydlidar_ros2_driver` requires the `YDLidar-SDK` library installed on your system.

```bash
# 1. Clone YDLidar-SDK
git clone https://github.com/YDLIDAR/YDLidar-SDK.git
cd YDLidar-SDK

# 2. Build and install
mkdir -p build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

---

## 3. Build ydlidar_ros2_driver

In your ROS 2 workspace (e.g. `~/ros2_ws`):

```bash
# 1. Create workspace directory
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src

# 2. Copy or clone this package into src/
# (If using this local repo: cp -r /path/to/ydlidar_ros2_driver ~/ros2_ws/src/)

# 3. Build with colcon
cd ~/ros2_ws
colcon build --symlink-install

# 4. Source setup
source ~/ros2_ws/install/setup.bash
```

To automatically source this workspace in new terminals:
```bash
echo "source ~/ros2_ws/install/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

---

## 4. Setup Serial Port Permissions / udev Alias

Give your user permission to access the serial port (`dialout` group) or create `/dev/ydlidar` symlink:

```bash
# Add current user to dialout group
sudo usermod -aG dialout $USER

# (Optional) Install udev rule to automatically create /dev/ydlidar symlink
cd ~/ros2_ws/src/ydlidar_ros2_driver
sudo chmod +x startup/initenv.sh
sudo ./startup/initenv.sh
```
*Note: After running this, re-plug the LiDAR USB cable into your computer.*

---

## 5. Configuration & Model Selection

Configuration files are located in `params/`:

| LiDAR Model | Parameter File |
| :--- | :--- |
| **Tmini Pro / Tmini Plus** | `params/Tmini.yaml` |
| **Tmini Plus SH** | `params/Tmini-Plus-SH.yaml` |
| **X2 / X2L** | `params/X2.yaml` |
| **X3** | `params/X3.yaml` |
| **X4** | `params/X4.yaml` |
| **X4 Pro** | `params/X4-Pro.yaml` |
| **G1** | `params/G1.yaml` |
| **G2** | `params/G2.yaml` |
| **G4** | `params/G4.yaml` |
| **G6** | `params/G6.yaml` |
| **GS2** | `params/GS2.yaml` |
| **GS5** | `params/GS5.yaml` |
| **TG15 / TG30 / TG50** | `params/TG.yaml` |
| **TEA** | `params/TEA.yaml` |
| **SDM15** | `params/sdm15.yaml` |
| **Default / Generic** | `params/ydlidar.yaml` |

### Tmini Plus Configuration (`params/Tmini.yaml`):
```yaml
ydlidar_ros2_driver_node:
  ros__parameters:
    port: /dev/ttyUSB0      # or /dev/ydlidar if udev rule installed
    frame_id: laser_frame
    ignore_array: ""
    baudrate: 230400
    lidar_type: 1           # 1 = TYPE_TRIANGLE
    device_type: 0          # 0 = SERIAL
    intensity: true
    intensity_bit: 8
    isSingleChannel: false
    frequency: 10.0
    sample_rate: 4
    abnormal_check_count: 4
    fixed_resolution: true
    reversion: true
    inverted: true
    auto_reconnect: true
    support_motor_dtr: false
    angle_max: 180.0
    angle_min: -180.0
    range_max: 12.0
    range_min: 0.03
    invalid_range_is_inf: false
    debug: false
```

---

## 6. Running the Driver

### For YDLidar Tmini Plus:
* **Driver only:**
  ```bash
  ros2 launch ydlidar_ros2_driver tmini_plus_launch.py
  ```
* **Driver + RViz2 view:**
  ```bash
  ros2 launch ydlidar_ros2_driver tmini_plus_view.py
  ```

### For Any Other Model:
* **Driver only (specify params_file):**
  ```bash
  ros2 launch ydlidar_ros2_driver ydlidar_launch.py params_file:=$(ros2 pkg prefix ydlidar_ros2_driver)/share/ydlidar_ros2_driver/params/X4.yaml
  ```
* **Driver + RViz2 view:**
  ```bash
  ros2 launch ydlidar_ros2_driver ydlidar_launch_view.py params_file:=$(ros2 pkg prefix ydlidar_ros2_driver)/share/ydlidar_ros2_driver/params/X4.yaml
  ```

### View Scan Data in Terminal:
```bash
# Using the driver client
ros2 run ydlidar_ros2_driver ydlidar_ros2_driver_client

# Or using ros2 topic echo
ros2 topic echo /scan
```

---

## 7. ROS 2 Interfaces

### Published Topics:
* `/scan` (`sensor_msgs/msg/LaserScan`): 2D laser scan data (QoS: `SensorDataQoS`, Best Effort).
* `/point_cloud` (`sensor_msgs/msg/PointCloud`): 2D point cloud representation.

### Services:
* `/start_scan` (`std_srvs/srv/Empty`): Turn on laser rotation and scanning.
* `/stop_scan` (`std_srvs/srv/Empty`): Turn off laser rotation and scanning.

---

## 8. Troubleshooting (ROS 2 Jazzy)

* **RViz2 shows "Incompatible QoS":**
  `ydlidar_ros2_driver` publishes using `rclcpp::SensorDataQoS()` (Best Effort reliability). The included `config/ydlidar.rviz` is already set to `Best Effort`. If creating a new RViz display, set `Topic -> Reliability Policy` to `Best Effort`.

* **Device Permission Denied (`/dev/ttyUSB0`):**
  Ensure your user is part of the `dialout` group: `sudo usermod -aG dialout $USER`, then log out and log back in.

* **Scan Drop / Old Timestamp in TF:**
  This version automatically detects uninitialized timestamps (`scan.stamp == 0`) and populates `header.stamp` with `node->get_clock()->now()`, preventing TF and RViz dropping errors.
