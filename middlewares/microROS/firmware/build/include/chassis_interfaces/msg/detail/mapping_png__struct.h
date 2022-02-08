// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/MappingPng.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__MAPPING_PNG__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__MAPPING_PNG__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'file_md5'
// Member 'png_image'
#include "rosidl_runtime_c/string.h"

// Struct defined in msg/MappingPng in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__MappingPng
{
  uint32_t height;
  uint32_t width;
  double origin_x;
  double origin_y;
  double resolution;
  uint32_t file_size;
  rosidl_runtime_c__String file_md5;
  rosidl_runtime_c__String png_image;
} chassis_interfaces__msg__MappingPng;

// Struct for a sequence of chassis_interfaces__msg__MappingPng.
typedef struct chassis_interfaces__msg__MappingPng__Sequence
{
  chassis_interfaces__msg__MappingPng * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__MappingPng__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__MAPPING_PNG__STRUCT_H_
