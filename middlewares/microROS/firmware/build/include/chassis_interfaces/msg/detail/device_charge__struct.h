// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/DeviceCharge.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__DEVICE_CHARGE__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__DEVICE_CHARGE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'command'
// Member 'map_id_list'
// Member 'strategy'
// Member 'workstation_id'
// Member 'charge_trace_id'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/DeviceCharge in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__DeviceCharge
{
  rosidl_runtime_c__String command;
  rosidl_runtime_c__String__Sequence map_id_list;
  uint8_t battery;
  rosidl_runtime_c__String strategy;
  rosidl_runtime_c__String workstation_id;
  rosidl_runtime_c__String charge_trace_id;
} chassis_interfaces__msg__DeviceCharge;

// Struct for a sequence of chassis_interfaces__msg__DeviceCharge.
typedef struct chassis_interfaces__msg__DeviceCharge__Sequence
{
  chassis_interfaces__msg__DeviceCharge * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__DeviceCharge__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__DEVICE_CHARGE__STRUCT_H_
