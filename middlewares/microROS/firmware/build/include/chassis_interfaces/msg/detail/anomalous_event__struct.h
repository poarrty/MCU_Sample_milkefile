// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/AnomalousEvent.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__ANOMALOUS_EVENT__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__ANOMALOUS_EVENT__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'event_type'
// Member 'event_data'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/AnomalousEvent in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__AnomalousEvent
{
  rosidl_runtime_c__String event_type;
  rosidl_runtime_c__String event_data;
} chassis_interfaces__msg__AnomalousEvent;

// Struct for a sequence of chassis_interfaces__msg__AnomalousEvent.
typedef struct chassis_interfaces__msg__AnomalousEvent__Sequence
{
  chassis_interfaces__msg__AnomalousEvent * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__AnomalousEvent__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__ANOMALOUS_EVENT__STRUCT_H_
