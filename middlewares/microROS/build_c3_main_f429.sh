#/bin/sh

source /opt/ros/foxy/setup.bash
source install/local_setup.bash

ros2 run micro_ros_setup build_firmware.sh $(pwd)/c3_main_f429_lib.cmake $(pwd)/c3_main_f429_lib.meta

mv firmware/build/include/rmw_microxrcedds_c/config.h ../../projects/c3_mcu_main_project/c3_mcu_main/pal/microros/config/rmw_microxrcedds_c/config.h
mv firmware/build/include/uxr/client/config.h ../../projects/c3_mcu_main_project/c3_mcu_main/pal/microros/config/uxr/client/config.h
mv firmware/build/libmicroros.a ../../projects/c3_mcu_main_project/c3_mcu_main/pal/microros/config/libmicroros.a