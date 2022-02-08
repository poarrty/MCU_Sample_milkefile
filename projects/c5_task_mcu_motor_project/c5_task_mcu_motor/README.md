# bingo_task_mcu_main

- ### 加密库安装
```bash
sudo apt install python3-pip
pip3 install pycrypto
```
- ### 编译microros固件库并编译代码（不可加-j8等多线程选项）
```bash
make microros
```
- ### 编译出用于OTA升级的IAP固件
```bash
make iap
```
- ### 编译并烧录固件
```bash
make flash
```
- ### 烧录带boot的固件
```bash
make download
```
- ### 用风机源码编译
```bash
make FAN_SOURCE_CODE=1
```

