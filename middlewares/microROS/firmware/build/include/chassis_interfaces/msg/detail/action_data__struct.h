// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/ActionData.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__ACTION_DATA__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__ACTION_DATA__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'data'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/ActionData in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__ActionData
{
  rosidl_runtime_c__String data;
} chassis_interfaces__msg__ActionData;

// Struct for a sequence of chassis_interfaces__msg__ActionData.
typedef struct chassis_interfaces__msg__ActionData__Sequence
{
  chassis_interfaces__msg__ActionData * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__ActionData__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__ACTION_DATA__STRUCT_H_
