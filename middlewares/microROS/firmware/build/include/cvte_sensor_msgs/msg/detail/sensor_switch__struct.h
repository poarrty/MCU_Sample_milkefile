// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from cvte_sensor_msgs:msg/SensorSwitch.idl
// generated code does not contain a copyright notice

#ifndef CVTE_SENSOR_MSGS__MSG__DETAIL__SENSOR_SWITCH__STRUCT_H_
#define CVTE_SENSOR_MSGS__MSG__DETAIL__SENSOR_SWITCH__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"

// Struct defined in msg/SensorSwitch in the package cvte_sensor_msgs.
typedef struct cvte_sensor_msgs__msg__SensorSwitch
{
  std_msgs__msg__Header header;
  uint8_t status;
} cvte_sensor_msgs__msg__SensorSwitch;

// Struct for a sequence of cvte_sensor_msgs__msg__SensorSwitch.
typedef struct cvte_sensor_msgs__msg__SensorSwitch__Sequence
{
  cvte_sensor_msgs__msg__SensorSwitch * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} cvte_sensor_msgs__msg__SensorSwitch__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CVTE_SENSOR_MSGS__MSG__DETAIL__SENSOR_SWITCH__STRUCT_H_
