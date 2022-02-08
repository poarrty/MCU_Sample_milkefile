// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/Ledcontrol.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__LEDCONTROL__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__LEDCONTROL__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'led_color'
// Member 'led_hz'
#include "rosidl_runtime_c/primitives_sequence.h"

// Struct defined in msg/Ledcontrol in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__Ledcontrol
{
  rosidl_runtime_c__uint32__Sequence led_color;
  rosidl_runtime_c__uint8__Sequence led_hz;
} chassis_interfaces__msg__Ledcontrol;

// Struct for a sequence of chassis_interfaces__msg__Ledcontrol.
typedef struct chassis_interfaces__msg__Ledcontrol__Sequence
{
  chassis_interfaces__msg__Ledcontrol * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__Ledcontrol__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__LEDCONTROL__STRUCT_H_
