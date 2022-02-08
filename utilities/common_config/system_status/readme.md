# 系统状态公共接口

## 使用说明

1.用到的hardfault保存需要用来数据库，使用前需要将键值数据库定义成`struct fdb_kvdb kvdb`

2.如果需要用到外部RAM，则打开STRING_RAM_EXT的宏

3.htop功能查询时间,单位为秒,查询时间为0时,关闭htop功能

4.需要系统时钟定时器,暂不支持systick

5.可自定义输出方式,如通过printf输出
