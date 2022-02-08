// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/PointCoordinate.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__POINT_COORDINATE__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__POINT_COORDINATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in msg/PointCoordinate in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__PointCoordinate
{
  double x;
  double y;
  double z;
  double yaw;
} chassis_interfaces__msg__PointCoordinate;

// Struct for a sequence of chassis_interfaces__msg__PointCoordinate.
typedef struct chassis_interfaces__msg__PointCoordinate__Sequence
{
  chassis_interfaces__msg__PointCoordinate * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__PointCoordinate__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__POINT_COORDINATE__STRUCT_H_
