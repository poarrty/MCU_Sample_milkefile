set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(PLATFORM_NAME "LwIP")

# SET HERE THE PATH TO YOUR C99 AND C++ COMPILERS
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_C_COMPILER_WORKS 1 CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS 1 CACHE INTERNAL "")

# SET HERE YOUR BUILDING FLAGS
# set(FLAGS "-O2 -ffunction-sections -fdata-sections -fno-exceptions -mcpu=cortex-m3 -nostdlib -mthumb --param max-inline-insns-single=500 -DF_CPU=84000000L -D'RCUTILS_LOG_MIN_SEVERITY=RCUTILS_LOG_MIN_SEVERITY_NONE'" CACHE STRING "" FORCE)
# set(ARCH_CPU_FLAGS "-mcpu=cortex-m3 -mthumb -DUSE_HAL_DRIVER -DSTM32F103xx -D_TIMEVAL_DEFINED -O0 -Wall -fdata-sections -ffunction-sections")
set(ARCH_CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DUSE_HAL_DRIVER -DSTM32F429xx -D_TIMEVAL_DEFINED -O0 -Wall -fdata-sections -ffunction-sections")
set(ARCH_OPT_FLAGS "")

# set(CMAKE_C_FLAGS_INIT "-std=c11 ${FLAGS} -DCLOCK_MONOTONIC=0 -D'__attribute__(x)='" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS_INIT "-std=c++11 ${FLAGS} -fno-rtti -DCLOCK_MONOTONIC=0 -D'__attribute__(x)='" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_INIT "-std=c11 ${ARCH_CPU_FLAGS} ${ARCH_OPT_FLAGS} -DCLOCK_MONOTONIC=0 -DRMW_UXRCE_FIXBUG  -DPLATFORM_NAME_FREERTOS" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_INIT "-std=c++14 ${ARCH_CPU_FLAGS} ${ARCH_OPT_FLAGS} -DCLOCK_MONOTONIC=0 -DRMW_UXRCE_FIXBUG  -DPLATFORM_NAME_FREERTOS" CACHE STRING "" FORCE)

include_directories(SYSTEM
${CMAKE_CURRENT_LIST_DIR}/../../projects/c3_mcu_main_project/c3_mcu_main/hal/core/Inc
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS/Source/CMSIS_RTOS_V2
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS/Source/include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS-Plus-POSIX/include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include/private
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include/FreeRTOS_POSIX
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include/FreeRTOS_POSIX/sys
${CMAKE_CURRENT_LIST_DIR}/../../drivers/stm32_fw_f4/STM32F4xx_HAL_Driver/Inc
${CMAKE_CURRENT_LIST_DIR}/../../drivers/stm32_fw_f4/CMSIS/Device/ST/STM32F4xx/Include
${CMAKE_CURRENT_LIST_DIR}/../../drivers/stm32_fw_f4/CMSIS/Include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/src/include/posix
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/src/include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/system
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/src/include/compat/posix
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/src/include/compat/posix/arpa
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/src/include/compat/posix/net
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/src/include/compat/posix/sys
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/LwIP/src/include/compat/stdc
)

set(__BIG_ENDIAN__ 0)
