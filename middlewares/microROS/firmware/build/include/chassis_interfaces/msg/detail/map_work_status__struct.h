// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/MapWorkStatus.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__MAP_WORK_STATUS__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__MAP_WORK_STATUS__STRUCT_H_

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
// Member 'location_state'
// Member 'location_operation_state'
// Member 'map_id'
// Member 'map_create_time'
#include "rosidl_runtime_c/string.h"
// Member 'coordinate'
#include "chassis_interfaces/msg/detail/point_coordinate__struct.h"

// Struct defined in msg/MapWorkStatus in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__MapWorkStatus
{
  rosidl_runtime_c__String work_status;
  rosidl_runtime_c__String location_state;
  rosidl_runtime_c__String location_operation_state;
  chassis_interfaces__msg__PointCoordinate coordinate;
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String map_create_time;
} chassis_interfaces__msg__MapWorkStatus;

// Struct for a sequence of chassis_interfaces__msg__MapWorkStatus.
typedef struct chassis_interfaces__msg__MapWorkStatus__Sequence
{
  chassis_interfaces__msg__MapWorkStatus * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__MapWorkStatus__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__MAP_WORK_STATUS__STRUCT_H_
