set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# SET HERE THE PATH TO YOUR C99 AND C++ COMPILERS
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_C_COMPILER_WORKS 1 CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS 1 CACHE INTERNAL "")

# SET HERE YOUR BUILDING FLAGS
# set(FLAGS "-O2 -ffunction-sections -fdata-sections -fno-exceptions -mcpu=cortex-m3 -nostdlib -mthumb --param max-inline-insns-single=500 -DF_CPU=84000000L -D'RCUTILS_LOG_MIN_SEVERITY=RCUTILS_LOG_MIN_SEVERITY_NONE'" CACHE STRING "" FORCE)
set(ARCH_CPU_FLAGS "-mcpu=cortex-m3 -mthumb -DUSE_HAL_DRIVER -DSTM32F103xx -D_TIMEVAL_DEFINED -O0 -Wall -fdata-sections -ffunction-sections")
set(ARCH_OPT_FLAGS "")

# set(CMAKE_C_FLAGS_INIT "-std=c11 ${FLAGS} -DCLOCK_MONOTONIC=0 -D'__attribute__(x)='" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS_INIT "-std=c++11 ${FLAGS} -fno-rtti -DCLOCK_MONOTONIC=0 -D'__attribute__(x)='" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_INIT "-std=c11 ${ARCH_CPU_FLAGS} ${ARCH_OPT_FLAGS} -DCLOCK_MONOTONIC=0 -DRMW_UXRCE_FIXBUG -DPLATFORM_NAME_FREERTOS" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_INIT "-std=c++14 ${ARCH_CPU_FLAGS} ${ARCH_OPT_FLAGS} -DCLOCK_MONOTONIC=0 -DRMW_UXRCE_FIXBUG -DPLATFORM_NAME_FREERTOS" CACHE STRING "" FORCE)

include_directories(SYSTEM
${CMAKE_CURRENT_LIST_DIR}/../../projects/c5_task_mcu_main_project/c5_task_mcu_main/hal/core/Inc
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS/Source/CMSIS_RTOS_V2
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS/Source/include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/FreeRTOS-Plus-POSIX/include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include/private
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include/FreeRTOS_POSIX
${CMAKE_CURRENT_LIST_DIR}/../../middlewares/FreeRTOS/include/FreeRTOS_POSIX/sys
)

set(__BIG_ENDIAN__ 0)
