## 文件夹说明：

#### 1、用来存放外设驱动代码；

#### 重新编译STM32CUBEMX后
#### 1、Makefile  删掉自动生成的包含的.c和.h，根据生成的/SRC增加的文件，例如添加了I2C.c，则添加$(EXTENSIONS_DIR)/Src/i2c.c \
#### 2、Inc/FreeRTOSConfig.h 回退去掉的一行 
#    #define INCLUDE_xTaskGetCurrentTaskHandle   1
#### 3、stm32f1xx_hal_can.c ==> HAL_CAN_Init ==> 改先退出睡眠模式再测标志位 
#    /* Exit from sleep mode */
#    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_SLEEP);
#    /* Request initialisation */
#    SET_BIT(hcan->Instance->MCR, CAN_MCR_INRQ);
#    while ((hcan->Instance->MSR & CAN_MSR_INAK) == 0U)
#    while ((hcan->Instance->MSR & CAN_MSR_SLAK) != 0U)
#### 4、/src/rtc.c:47 hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;#
#    改为hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE; PC13才能当IO使用
#### 5、/src/stm32f1xx_it.c Hardware_fault()注释掉
#### 6、Makefile STM32F103RDTx_FLASH_IAP.ld补上自动去掉的_IAP
#### 7、删除core/Drivers和core/Middlewares,使用工程外的库
#### 8、删除  -DSTM32F103xE 使用xG系列