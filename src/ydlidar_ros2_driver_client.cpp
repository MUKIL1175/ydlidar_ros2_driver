/*
 *  YDLIDAR SYSTEM
 *  YDLIDAR ROS 2 Node Client
 *
 *  Copyright 2017 - 2024 EAI TEAM
 *  http://www.eaibot.com
 *
 *  Adapted for ROS 2 Jazzy & Ubuntu 24.04
 */

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG(x) ((x) * 180.0 / M_PI)

static void scanCb(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
  size_t count = scan->ranges.size();
  RCLCPP_INFO(rclcpp::get_logger("ydlidar_client"),
              "[YDLIDAR INFO]: I heard a laser scan %s [%zu points]:",
              scan->header.frame_id.c_str(), count);
  RCLCPP_INFO(rclcpp::get_logger("ydlidar_client"),
              "[YDLIDAR INFO]: angle_range : [%.2f deg, %.2f deg]",
              RAD2DEG(scan->angle_min), RAD2DEG(scan->angle_max));

  for (size_t i = 0; i < count; i++) {
    float degree = RAD2DEG(scan->angle_min + scan->angle_increment * i);
    printf("[YDLIDAR INFO]: angle-distance : [%.2f deg, %.4f m]\n", degree, scan->ranges[i]);
  }
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  auto node = rclcpp::Node::make_shared("ydlidar_ros2_driver_client");

  auto lidar_info_sub = node->create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", rclcpp::SensorDataQoS(), scanCb);

  rclcpp::spin(node);

  rclcpp::shutdown();

  return 0;
}
