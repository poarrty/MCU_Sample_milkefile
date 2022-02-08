// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/RunTaskCondition.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__RUN_TASK_CONDITION__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__RUN_TASK_CONDITION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'condition_type'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/RunTaskCondition in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__RunTaskCondition
{
  rosidl_runtime_c__String condition_type;
  bool condition_result;
} chassis_interfaces__msg__RunTaskCondition;

// Struct for a sequence of chassis_interfaces__msg__RunTaskCondition.
typedef struct chassis_interfaces__msg__RunTaskCondition__Sequence
{
  chassis_interfaces__msg__RunTaskCondition * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__RunTaskCondition__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__RUN_TASK_CONDITION__STRUCT_H_
