// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/SelfCheck.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__SELF_CHECK__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__SELF_CHECK__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'sub_execute_id'
// Member 'check_result'
// Member 'check_msg'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/SelfCheck in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__SelfCheck
{
  rosidl_runtime_c__String sub_execute_id;
  rosidl_runtime_c__String check_result;
  rosidl_runtime_c__String check_msg;
} chassis_interfaces__msg__SelfCheck;

// Struct for a sequence of chassis_interfaces__msg__SelfCheck.
typedef struct chassis_interfaces__msg__SelfCheck__Sequence
{
  chassis_interfaces__msg__SelfCheck * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__SelfCheck__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__SELF_CHECK__STRUCT_H_
