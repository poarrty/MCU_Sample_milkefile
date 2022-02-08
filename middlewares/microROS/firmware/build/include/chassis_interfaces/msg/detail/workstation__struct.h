// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/Workstation.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__WORKSTATION__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__WORKSTATION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'command'
// Member 'map_id'
// Member 'point_id'
// Member 'workstation_id'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/Workstation in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__Workstation
{
  rosidl_runtime_c__String command;
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String point_id;
  rosidl_runtime_c__String workstation_id;
} chassis_interfaces__msg__Workstation;

// Struct for a sequence of chassis_interfaces__msg__Workstation.
typedef struct chassis_interfaces__msg__Workstation__Sequence
{
  chassis_interfaces__msg__Workstation * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__Workstation__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__WORKSTATION__STRUCT_H_
