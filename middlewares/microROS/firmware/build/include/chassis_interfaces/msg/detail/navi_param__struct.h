// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/NaviParam.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__NAVI_PARAM__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__NAVI_PARAM__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in msg/NaviParam in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__NaviParam
{
  bool is_forward;
  bool is_final_rotate;
  uint32_t path_type;
  bool is_abs_reach;
} chassis_interfaces__msg__NaviParam;

// Struct for a sequence of chassis_interfaces__msg__NaviParam.
typedef struct chassis_interfaces__msg__NaviParam__Sequence
{
  chassis_interfaces__msg__NaviParam * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__NaviParam__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__NAVI_PARAM__STRUCT_H_
