#!/usr/bin/env python3
# Copyright 2020, EAIBOT
# Adapted for ROS 2 Jazzy & Ubuntu 24.04
# Licensed under the Apache License, Version 2.0

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    driver_node = Node(
        package='ydlidar_ros2_driver',
        executable='ydlidar_ros2_driver_node',
        name='ydlidar_ros2_driver_node',
        output='screen',
        emulate_tty=True,
        namespace='/'
    )

    tf2_node = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='static_tf_pub_laser',
        arguments=[
            '--x', '0',
            '--y', '0',
            '--z', '0.02',
            '--qx', '0',
            '--qy', '0',
            '--qz', '0',
            '--qw', '1',
            '--frame-id', 'base_link',
            '--child-frame-id', 'laser_frame'
        ]
    )

    return LaunchDescription([
        driver_node,
        tf2_node,
    ])
