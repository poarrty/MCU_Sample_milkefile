// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/NaviPointCmd.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__NAVI_POINT_CMD__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__NAVI_POINT_CMD__STRUCT_H_

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
// Member 'point_id'
// Member 'point_type'
#include "rosidl_runtime_c/string.h"
// Member 'coordinate'
#include "chassis_interfaces/msg/detail/point_coordinate__struct.h"
// Member 'navi_param'
#include "chassis_interfaces/msg/detail/navi_param__struct.h"

// Struct defined in msg/NaviPointCmd in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__NaviPointCmd
{
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String point_id;
  rosidl_runtime_c__String point_type;
  chassis_interfaces__msg__PointCoordinate coordinate;
  chassis_interfaces__msg__NaviParam navi_param;
} chassis_interfaces__msg__NaviPointCmd;

// Struct for a sequence of chassis_interfaces__msg__NaviPointCmd.
typedef struct chassis_interfaces__msg__NaviPointCmd__Sequence
{
  chassis_interfaces__msg__NaviPointCmd * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__NaviPointCmd__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__NAVI_POINT_CMD__STRUCT_H_
