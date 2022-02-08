// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/CallLift.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__CALL_LIFT__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__CALL_LIFT__STRUCT_H_

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
// Member 'target_map_id'
// Member 'lift_id_list'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/CallLift in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__CallLift
{
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String target_map_id;
  rosidl_runtime_c__String__Sequence lift_id_list;
} chassis_interfaces__msg__CallLift;

// Struct for a sequence of chassis_interfaces__msg__CallLift.
typedef struct chassis_interfaces__msg__CallLift__Sequence
{
  chassis_interfaces__msg__CallLift * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__CallLift__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__CALL_LIFT__STRUCT_H_
