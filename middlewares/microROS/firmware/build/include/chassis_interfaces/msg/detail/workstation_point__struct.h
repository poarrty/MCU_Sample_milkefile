// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/WorkstationPoint.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__WORKSTATION_POINT__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__WORKSTATION_POINT__STRUCT_H_

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
// Member 'device_id'
#include "rosidl_runtime_c/string.h"
// Member 'pile_point_coordinate'
// Member 'workstation_point_coordinate'
#include "chassis_interfaces/msg/detail/point_coordinate__struct.h"

// Struct defined in msg/WorkstationPoint in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__WorkstationPoint
{
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String point_id;
  rosidl_runtime_c__String device_id;
  chassis_interfaces__msg__PointCoordinate pile_point_coordinate;
  chassis_interfaces__msg__PointCoordinate workstation_point_coordinate;
} chassis_interfaces__msg__WorkstationPoint;

// Struct for a sequence of chassis_interfaces__msg__WorkstationPoint.
typedef struct chassis_interfaces__msg__WorkstationPoint__Sequence
{
  chassis_interfaces__msg__WorkstationPoint * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__WorkstationPoint__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__WORKSTATION_POINT__STRUCT_H_
