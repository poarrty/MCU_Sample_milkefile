// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/MapPointList.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__MAP_POINT_LIST__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__MAP_POINT_LIST__STRUCT_H_

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
#include "rosidl_runtime_c/string.h"
// Member 'vec_point'
#include "chassis_interfaces/msg/detail/point_info__struct.h"

// Struct defined in msg/MapPointList in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__MapPointList
{
  rosidl_runtime_c__String map_id;
  chassis_interfaces__msg__PointInfo__Sequence vec_point;
} chassis_interfaces__msg__MapPointList;

// Struct for a sequence of chassis_interfaces__msg__MapPointList.
typedef struct chassis_interfaces__msg__MapPointList__Sequence
{
  chassis_interfaces__msg__MapPointList * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__MapPointList__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__MAP_POINT_LIST__STRUCT_H_
