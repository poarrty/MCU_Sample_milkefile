// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/SwitchMapInitCmd.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__SWITCH_MAP_INIT_CMD__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__SWITCH_MAP_INIT_CMD__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'switch_map_id'
// Member 'switch_point_id'
// Member 'current_point_id'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/SwitchMapInitCmd in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__SwitchMapInitCmd
{
  rosidl_runtime_c__String switch_map_id;
  rosidl_runtime_c__String switch_point_id;
  rosidl_runtime_c__String current_point_id;
} chassis_interfaces__msg__SwitchMapInitCmd;

// Struct for a sequence of chassis_interfaces__msg__SwitchMapInitCmd.
typedef struct chassis_interfaces__msg__SwitchMapInitCmd__Sequence
{
  chassis_interfaces__msg__SwitchMapInitCmd * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__SwitchMapInitCmd__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__SWITCH_MAP_INIT_CMD__STRUCT_H_
