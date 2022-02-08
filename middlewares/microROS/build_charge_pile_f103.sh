#/bin/sh

source /opt/ros/foxy/setup.bash
source install/local_setup.bash

ros2 run micro_ros_setup build_firmware.sh $(pwd)/charge_pile_f103_microros_lib.cmake $(pwd)/charge_pile_f103_microros_lib.meta

mv firmware/build/include/rmw_microxrcedds_c/config.h ../../projects/robot_charge_pile_mcu_main_project/robot_charge_pile_mcu_main/middlewares/microros/rmw_microxrcedds_c/config.h
mv firmware/build/include/uxr/client/config.h ../../projects/robot_charge_pile_mcu_main_project/robot_charge_pile_mcu_main/middlewares/microros/uxr/client/config.h
mv firmware/build/libmicroros.a ../../projects/robot_charge_pile_mcu_main_project/robot_charge_pile_mcu_main/middlewares/microros/lib/libmicroros.a