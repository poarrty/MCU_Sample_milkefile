// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/NaviInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__NAVI_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__NAVI_INFO__STRUCT_H_

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
// Member 'point_info'
#include "chassis_interfaces/msg/detail/navi_point_info__struct.h"
// Member 'path_info'
#include "chassis_interfaces/msg/detail/navi_path_info__struct.h"
// Member 'region_info'
#include "chassis_interfaces/msg/detail/navi_region_info__struct.h"

// Struct defined in msg/NaviInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__NaviInfo
{
  rosidl_runtime_c__String map_id;
  chassis_interfaces__msg__NaviPointInfo point_info;
  chassis_interfaces__msg__NaviPathInfo path_info;
  chassis_interfaces__msg__NaviRegionInfo region_info;
} chassis_interfaces__msg__NaviInfo;

// Struct for a sequence of chassis_interfaces__msg__NaviInfo.
typedef struct chassis_interfaces__msg__NaviInfo__Sequence
{
  chassis_interfaces__msg__NaviInfo * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__NaviInfo__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__NAVI_INFO__STRUCT_H_
