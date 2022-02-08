// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/PointInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__POINT_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__POINT_INFO__STRUCT_H_

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
// Member 'coordinate'
#include "chassis_interfaces/msg/detail/point_coordinate__struct.h"
// Member 'extend_attributes'
#include "chassis_interfaces/msg/detail/string_map__struct.h"

// Struct defined in msg/PointInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__PointInfo
{
  rosidl_runtime_c__String point_id;
  rosidl_runtime_c__String point_type;
  chassis_interfaces__msg__PointCoordinate coordinate;
  chassis_interfaces__msg__StringMap__Sequence extend_attributes;
} chassis_interfaces__msg__PointInfo;

// Struct for a sequence of chassis_interfaces__msg__PointInfo.
typedef struct chassis_interfaces__msg__PointInfo__Sequence
{
  chassis_interfaces__msg__PointInfo * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__PointInfo__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__POINT_INFO__STRUCT_H_
