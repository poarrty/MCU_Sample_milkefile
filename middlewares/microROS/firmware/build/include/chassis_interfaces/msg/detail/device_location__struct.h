// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/DeviceLocation.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__DEVICE_LOCATION__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__DEVICE_LOCATION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'map_id'
// Member 'state_time'
#include "rosidl_runtime_c/string.h"
// Member 'point'
#include "chassis_interfaces/msg/detail/point_coordinate__struct.h"

// Struct defined in msg/DeviceLocation in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__DeviceLocation
{
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String state_time;
  chassis_interfaces__msg__PointCoordinate point;
} chassis_interfaces__msg__DeviceLocation;

// Struct for a sequence of chassis_interfaces__msg__DeviceLocation.
typedef struct chassis_interfaces__msg__DeviceLocation__Sequence
{
  chassis_interfaces__msg__DeviceLocation * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__DeviceLocation__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__DEVICE_LOCATION__STRUCT_H_
