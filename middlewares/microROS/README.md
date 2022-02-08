# microros公用层使用说明

1.当与C++约定新的新的自定义协议，将更新后的自定义消息类型放入/fireware/mcu_ws/ros2中，运行对应mcu的脚本

2.运行后得到新的/include文件夹和.a文件

3.将工程需要用的.a文件、因为meta文件配置不同导致的config.h文件移动到自己的工程（需要放在自己的rmw_microxrecdds_c文件夹下）
