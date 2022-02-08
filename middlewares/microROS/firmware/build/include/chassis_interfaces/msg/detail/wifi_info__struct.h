// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/WifiInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__WIFI_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__WIFI_INFO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'ssid'
// Member 'bssid'
// Member 'psk'
// Member 'key_mqmt'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/WifiInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__WifiInfo
{
  rosidl_runtime_c__String ssid;
  rosidl_runtime_c__String bssid;
  rosidl_runtime_c__String psk;
  bool scan_ssid;
  rosidl_runtime_c__String key_mqmt;
} chassis_interfaces__msg__WifiInfo;

// Struct for a sequence of chassis_interfaces__msg__WifiInfo.
typedef struct chassis_interfaces__msg__WifiInfo__Sequence
{
  chassis_interfaces__msg__WifiInfo * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__WifiInfo__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__WIFI_INFO__STRUCT_H_
