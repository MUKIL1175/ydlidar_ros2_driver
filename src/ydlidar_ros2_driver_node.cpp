/*
 *  YDLIDAR SYSTEM
 *  YDLIDAR ROS 2 Node
 *
 *  Copyright 2017 - 2024 EAI TEAM
 *  http://www.eaibot.com
 *
 *  Adapted for ROS 2 Jazzy & Ubuntu 24.04
 */

#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif

#include "src/CYdLidar.h"
#include <cmath>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <limits>
#include <csignal>

#include "rcutils/time.h"
#include "rmw/types.h"
#include "rclcpp/clock.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/time_source.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/point_cloud.hpp"
#include "geometry_msgs/msg/point32.hpp"
#include "std_srvs/srv/empty.hpp"

#define ROS2DriverVersion "1.0.3"

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);

  auto node = rclcpp::Node::make_shared("ydlidar_ros2_driver_node");

  RCLCPP_INFO(node->get_logger(), "[YDLIDAR INFO] Current ROS Driver Version: %s", ROS2DriverVersion);

  CYdLidar laser;

  ////////////////////// string properties /////////////////////
  std::string port = "/dev/ydlidar";
  node->declare_parameter<std::string>("port", port);
  node->get_parameter("port", port);
  laser.setlidaropt(LidarPropSerialPort, port.c_str(), port.size());

  std::string ignore_array = "";
  node->declare_parameter<std::string>("ignore_array", ignore_array);
  node->get_parameter("ignore_array", ignore_array);
  laser.setlidaropt(LidarPropIgnoreArray, ignore_array.c_str(), ignore_array.size());

  std::string frame_id = "laser_frame";
  node->declare_parameter<std::string>("frame_id", frame_id);
  node->get_parameter("frame_id", frame_id);

  ////////////////////// int properties /////////////////////
  int baudrate = 230400;
  node->declare_parameter<int>("baudrate", baudrate);
  node->get_parameter("baudrate", baudrate);
  laser.setlidaropt(LidarPropSerialBaudrate, &baudrate, sizeof(int));

  int lidar_type = TYPE_TRIANGLE;
  node->declare_parameter<int>("lidar_type", lidar_type);
  node->get_parameter("lidar_type", lidar_type);
  laser.setlidaropt(LidarPropLidarType, &lidar_type, sizeof(int));

  int device_type = YDLIDAR_TYPE_SERIAL;
  node->declare_parameter<int>("device_type", device_type);
  node->get_parameter("device_type", device_type);
  laser.setlidaropt(LidarPropDeviceType, &device_type, sizeof(int));

  int sample_rate = 9;
  node->declare_parameter<int>("sample_rate", sample_rate);
  node->get_parameter("sample_rate", sample_rate);
  laser.setlidaropt(LidarPropSampleRate, &sample_rate, sizeof(int));

  int abnormal_check_count = 4;
  node->declare_parameter<int>("abnormal_check_count", abnormal_check_count);
  node->get_parameter("abnormal_check_count", abnormal_check_count);
  laser.setlidaropt(LidarPropAbnormalCheckCount, &abnormal_check_count, sizeof(int));

  int intensity_bit = 0;
  node->declare_parameter<int>("intensity_bit", intensity_bit);
  node->get_parameter("intensity_bit", intensity_bit);
  laser.setlidaropt(LidarPropIntenstiyBit, &intensity_bit, sizeof(int));

  ////////////////////// bool properties /////////////////////
  bool fixed_resolution = false;
  node->declare_parameter<bool>("fixed_resolution", fixed_resolution);
  node->get_parameter("fixed_resolution", fixed_resolution);
  laser.setlidaropt(LidarPropFixedResolution, &fixed_resolution, sizeof(bool));

  bool reversion = true;
  node->declare_parameter<bool>("reversion", reversion);
  node->get_parameter("reversion", reversion);
  laser.setlidaropt(LidarPropReversion, &reversion, sizeof(bool));

  bool inverted = true;
  node->declare_parameter<bool>("inverted", inverted);
  node->get_parameter("inverted", inverted);
  laser.setlidaropt(LidarPropInverted, &inverted, sizeof(bool));

  bool auto_reconnect = true;
  node->declare_parameter<bool>("auto_reconnect", auto_reconnect);
  node->get_parameter("auto_reconnect", auto_reconnect);
  laser.setlidaropt(LidarPropAutoReconnect, &auto_reconnect, sizeof(bool));

  bool isSingleChannel = false;
  node->declare_parameter<bool>("isSingleChannel", isSingleChannel);
  node->get_parameter("isSingleChannel", isSingleChannel);
  laser.setlidaropt(LidarPropSingleChannel, &isSingleChannel, sizeof(bool));

  bool intensity = false;
  node->declare_parameter<bool>("intensity", intensity);
  node->get_parameter("intensity", intensity);
  laser.setlidaropt(LidarPropIntenstiy, &intensity, sizeof(bool));

  bool support_motor_dtr = false;
  node->declare_parameter<bool>("support_motor_dtr", support_motor_dtr);
  node->get_parameter("support_motor_dtr", support_motor_dtr);
  laser.setlidaropt(LidarPropSupportMotorDtrCtrl, &support_motor_dtr, sizeof(bool));

  bool debug = false;
  node->declare_parameter<bool>("debug", debug);
  node->get_parameter("debug", debug);
  laser.setEnableDebug(debug);

  ////////////////////// float/double properties /////////////////////
  // In ROS 2, floating point parameters are doubles in rclcpp/YAML.
  // We declare/get as double to avoid type mismatches, then cast to float for SDK.
  double angle_max_d = 180.0;
  node->declare_parameter<double>("angle_max", angle_max_d);
  node->get_parameter("angle_max", angle_max_d);
  float angle_max_f = static_cast<float>(angle_max_d);
  laser.setlidaropt(LidarPropMaxAngle, &angle_max_f, sizeof(float));

  double angle_min_d = -180.0;
  node->declare_parameter<double>("angle_min", angle_min_d);
  node->get_parameter("angle_min", angle_min_d);
  float angle_min_f = static_cast<float>(angle_min_d);
  laser.setlidaropt(LidarPropMinAngle, &angle_min_f, sizeof(float));

  double range_max_d = 64.0;
  node->declare_parameter<double>("range_max", range_max_d);
  node->get_parameter("range_max", range_max_d);
  float range_max_f = static_cast<float>(range_max_d);
  laser.setlidaropt(LidarPropMaxRange, &range_max_f, sizeof(float));

  double range_min_d = 0.1;
  node->declare_parameter<double>("range_min", range_min_d);
  node->get_parameter("range_min", range_min_d);
  float range_min_f = static_cast<float>(range_min_d);
  laser.setlidaropt(LidarPropMinRange, &range_min_f, sizeof(float));

  double frequency_d = 10.0;
  node->declare_parameter<double>("frequency", frequency_d);
  node->get_parameter("frequency", frequency_d);
  float frequency_f = static_cast<float>(frequency_d);
  laser.setlidaropt(LidarPropScanFrequency, &frequency_f, sizeof(float));

  bool invalid_range_is_inf = false;
  node->declare_parameter<bool>("invalid_range_is_inf", invalid_range_is_inf);
  node->get_parameter("invalid_range_is_inf", invalid_range_is_inf);

  // Initialize LiDAR
  bool ret = laser.initialize();
  if (ret) {
    // GS work mode settings (ignored for non-GS lidars)
    int m1_mode = 0;
    node->declare_parameter<int>("m1_mode", m1_mode);
    node->get_parameter("m1_mode", m1_mode);
    laser.setWorkMode(m1_mode, 0x01);

    int m2_mode = 0;
    node->declare_parameter<int>("m2_mode", m2_mode);
    node->get_parameter("m2_mode", m2_mode);
    laser.setWorkMode(m2_mode, 0x02);

    int m3_mode = 1;
    node->declare_parameter<int>("m3_mode", m3_mode);
    node->get_parameter("m3_mode", m3_mode);
    laser.setWorkMode(m3_mode, 0x04);

    // Start scanning
    ret = laser.turnOn();
  } else {
    RCLCPP_ERROR(node->get_logger(), "%s", laser.DescribeError());
  }

  // Publishers with SensorDataQoS (Best Effort, Volatile)
  auto laser_pub = node->create_publisher<sensor_msgs::msg::LaserScan>("scan", rclcpp::SensorDataQoS());
  auto pc_pub = node->create_publisher<sensor_msgs::msg::PointCloud>("point_cloud", rclcpp::SensorDataQoS());

  // Services with standard callback signature
  auto stop_scan_service =
    [&laser](const std::shared_ptr<rmw_request_id_t> /*request_header*/,
             const std::shared_ptr<std_srvs::srv::Empty::Request> /*req*/,
             std::shared_ptr<std_srvs::srv::Empty::Response> /*response*/) -> bool {
      return laser.turnOff();
    };
  auto stop_service = node->create_service<std_srvs::srv::Empty>("stop_scan", stop_scan_service);

  auto start_scan_service =
    [&laser](const std::shared_ptr<rmw_request_id_t> /*request_header*/,
             const std::shared_ptr<std_srvs::srv::Empty::Request> /*req*/,
             std::shared_ptr<std_srvs::srv::Empty::Response> /*response*/) -> bool {
      return laser.turnOn();
    };
  auto start_service = node->create_service<std_srvs::srv::Empty>("start_scan", start_scan_service);

  rclcpp::WallRate loop_rate(30);

  while (ret && rclcpp::ok()) {
    LaserScan scan;

    if (laser.doProcessSimple(scan)) {
      auto scan_msg = std::make_shared<sensor_msgs::msg::LaserScan>();
      auto pc_msg = std::make_shared<sensor_msgs::msg::PointCloud>();

      // Timestamping: fallback to ROS clock now() if scan.stamp is 0 to avoid TF/RViz drop
      if (scan.stamp != 0) {
        scan_msg->header.stamp = rclcpp::Time(static_cast<int64_t>(scan.stamp));
      } else {
        scan_msg->header.stamp = node->get_clock()->now();
      }

      scan_msg->header.frame_id = frame_id;
      pc_msg->header = scan_msg->header;

      scan_msg->angle_min = scan.config.min_angle;
      scan_msg->angle_max = scan.config.max_angle;
      scan_msg->angle_increment = scan.config.angle_increment;
      scan_msg->scan_time = scan.config.scan_time;
      scan_msg->time_increment = scan.config.time_increment;
      scan_msg->range_min = scan.config.min_range;
      scan_msg->range_max = scan.config.max_range;

      if (scan.config.angle_increment > 0.0f) {
        int size = static_cast<int>((scan.config.max_angle - scan.config.min_angle) / scan.config.angle_increment) + 1;
        float default_range = invalid_range_is_inf ? std::numeric_limits<float>::infinity() : 0.0f;
        std::vector<float> ranges(size, default_range);
        std::vector<float> intensities(size, 0.0f);

        pc_msg->channels.resize(2);
        int idx_intensity = 0;
        pc_msg->channels[idx_intensity].name = "intensities";
        int idx_timestamp = 1;
        pc_msg->channels[idx_timestamp].name = "stamps";

        for (size_t i = 0; i < scan.points.size(); ++i) {
          int index = std::ceil((scan.points[i].angle - scan.config.min_angle) / scan.config.angle_increment);
          if (index >= 0 && index < size) {
            if (scan.points[i].range >= scan.config.min_range && scan.points[i].range <= scan.config.max_range) {
              ranges[index] = scan.points[i].range;
              intensities[index] = scan.points[i].intensity;
            }
          }

          if (scan.points[i].range >= scan.config.min_range && scan.points[i].range <= scan.config.max_range) {
            geometry_msgs::msg::Point32 point;
            point.x = scan.points[i].range * std::cos(scan.points[i].angle);
            point.y = scan.points[i].range * std::sin(scan.points[i].angle);
            point.z = 0.0f;
            pc_msg->points.push_back(point);
            pc_msg->channels[idx_intensity].values.push_back(scan.points[i].intensity);
            pc_msg->channels[idx_timestamp].values.push_back(static_cast<float>(i) * scan.config.time_increment);
          }
        }

        scan_msg->ranges = std::move(ranges);
        scan_msg->intensities = std::move(intensities);

        laser_pub->publish(*scan_msg);
        pc_pub->publish(*pc_msg);
      }
    } else {
      RCLCPP_ERROR(node->get_logger(), "Failed to get scan");
    }

    if (!rclcpp::ok()) {
      break;
    }
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  RCLCPP_INFO(node->get_logger(), "[YDLIDAR INFO] Now YDLIDAR is stopping .......");
  laser.turnOff();
  laser.disconnecting();
  rclcpp::shutdown();

  return 0;
}
