// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/FirmwareInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__FIRMWARE_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__FIRMWARE_INFO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'app_key'
// Member 'version'
// Member 'platform'
// Member 'upgrade_service_name'
// Member 'updating_info_service_name'
// Member 'upgrade_progress_topic_name'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/FirmwareInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__FirmwareInfo
{
  std_msgs__msg__Header header;
  uint8_t priority;
  uint32_t package_size;
  rosidl_runtime_c__String app_key;
  rosidl_runtime_c__String version;
  rosidl_runtime_c__String platform;
  rosidl_runtime_c__String upgrade_service_name;
  rosidl_runtime_c__String updating_info_service_name;
  rosidl_runtime_c__String upgrade_progress_topic_name;
} chassis_interfaces__msg__FirmwareInfo;

// Struct for a sequence of chassis_interfaces__msg__FirmwareInfo.
typedef struct chassis_interfaces__msg__FirmwareInfo__Sequence
{
  chassis_interfaces__msg__FirmwareInfo * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__FirmwareInfo__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__FIRMWARE_INFO__STRUCT_H_
