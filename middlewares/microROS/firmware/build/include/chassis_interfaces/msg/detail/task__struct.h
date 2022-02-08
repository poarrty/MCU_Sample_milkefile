// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/Task.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__TASK__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__TASK__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'task_type'
// Member 'sub_execute_id'
// Member 'map_id_list'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/Task in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__Task
{
  rosidl_runtime_c__String task_type;
  rosidl_runtime_c__String sub_execute_id;
  rosidl_runtime_c__String__Sequence map_id_list;
} chassis_interfaces__msg__Task;

// Struct for a sequence of chassis_interfaces__msg__Task.
typedef struct chassis_interfaces__msg__Task__Sequence
{
  chassis_interfaces__msg__Task * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__Task__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__TASK__STRUCT_H_
