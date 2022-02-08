// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from cvte_sensor_msgs:msg/Encoding.idl
// generated code does not contain a copyright notice

#ifndef CVTE_SENSOR_MSGS__MSG__DETAIL__ENCODING__STRUCT_H_
#define CVTE_SENSOR_MSGS__MSG__DETAIL__ENCODING__STRUCT_H_

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
// Member 'encoding_num'
#include "rosidl_runtime_c/primitives_sequence.h"

// Struct defined in msg/Encoding in the package cvte_sensor_msgs.
typedef struct cvte_sensor_msgs__msg__Encoding
{
  std_msgs__msg__Header header;
  rosidl_runtime_c__int64__Sequence encoding_num;
  int64_t encoding_circle;
} cvte_sensor_msgs__msg__Encoding;

// Struct for a sequence of cvte_sensor_msgs__msg__Encoding.
typedef struct cvte_sensor_msgs__msg__Encoding__Sequence
{
  cvte_sensor_msgs__msg__Encoding * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} cvte_sensor_msgs__msg__Encoding__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CVTE_SENSOR_MSGS__MSG__DETAIL__ENCODING__STRUCT_H_
