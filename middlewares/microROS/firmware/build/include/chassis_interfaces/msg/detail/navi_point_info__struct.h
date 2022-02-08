// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/NaviPointInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__NAVI_POINT_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__NAVI_POINT_INFO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'point_id'
// Member 'point_type'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/NaviPointInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__NaviPointInfo
{
  rosidl_runtime_c__String point_id;
  rosidl_runtime_c__String point_type;
} chassis_interfaces__msg__NaviPointInfo;

// Struct for a sequence of chassis_interfaces__msg__NaviPointInfo.
typedef struct chassis_interfaces__msg__NaviPointInfo__Sequence
{
  chassis_interfaces__msg__NaviPointInfo * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__NaviPointInfo__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__NAVI_POINT_INFO__STRUCT_H_
