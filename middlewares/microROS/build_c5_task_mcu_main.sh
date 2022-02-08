#/bin/sh

source /opt/ros/foxy/setup.bash
source install/local_setup.bash

ros2 run micro_ros_setup build_firmware.sh $(pwd)/c5_task_mcu_main_lib.cmake $(pwd)/c5_task_mcu_main_lib.meta

mv firmware/build/include/rmw_microxrcedds_c/config.h ../../projects/c5_task_mcu_main_project/c5_task_mcu_main/middlewares/microros/rmw_microxrcedds_c/config.h
mv firmware/build/include/uxr/client/config.h ../../projects/c5_task_mcu_main_project/c5_task_mcu_main/middlewares/microros/uxr/client/config.h
mv firmware/build/libmicroros.a ../../projects/c5_task_mcu_main_project/c5_task_mcu_main/middlewares/microros/lib/libmicroros.a