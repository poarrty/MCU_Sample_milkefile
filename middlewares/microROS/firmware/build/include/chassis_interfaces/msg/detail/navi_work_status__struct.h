// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/NaviWorkStatus.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__NAVI_WORK_STATUS__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__NAVI_WORK_STATUS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'work_status'
// Member 'navigation_state'
// Member 'lift_state'
#include "rosidl_runtime_c/string.h"
// Member 'navigation_info'
#include "chassis_interfaces/msg/detail/navi_info__struct.h"

// Struct defined in msg/NaviWorkStatus in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__NaviWorkStatus
{
  rosidl_runtime_c__String work_status;
  rosidl_runtime_c__String navigation_state;
  chassis_interfaces__msg__NaviInfo navigation_info;
  rosidl_runtime_c__String lift_state;
} chassis_interfaces__msg__NaviWorkStatus;

// Struct for a sequence of chassis_interfaces__msg__NaviWorkStatus.
typedef struct chassis_interfaces__msg__NaviWorkStatus__Sequence
{
  chassis_interfaces__msg__NaviWorkStatus * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__NaviWorkStatus__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__NAVI_WORK_STATUS__STRUCT_H_
