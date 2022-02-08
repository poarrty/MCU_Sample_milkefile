// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/GetMapPng.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__GET_MAP_PNG__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__GET_MAP_PNG__STRUCT_H_

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
#include "rosidl_runtime_c/string.h"

// Struct defined in srv/GetMapPng in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetMapPng_Request
{
  rosidl_runtime_c__String map_id;
} chassis_interfaces__srv__GetMapPng_Request;

// Struct for a sequence of chassis_interfaces__srv__GetMapPng_Request.
typedef struct chassis_interfaces__srv__GetMapPng_Request__Sequence
{
  chassis_interfaces__srv__GetMapPng_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetMapPng_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'msg'
// Member 'map_name'
// Member 'file_md5'
// Member 'png_image'
// already included above
// #include "rosidl_runtime_c/string.h"

// Struct defined in srv/GetMapPng in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetMapPng_Response
{
  uint16_t code;
  rosidl_runtime_c__String msg;
  rosidl_runtime_c__String map_name;
  uint32_t height;
  uint32_t width;
  double origin_x;
  double origin_y;
  double resolution;
  uint32_t file_size;
  rosidl_runtime_c__String file_md5;
  rosidl_runtime_c__String png_image;
} chassis_interfaces__srv__GetMapPng_Response;

// Struct for a sequence of chassis_interfaces__srv__GetMapPng_Response.
typedef struct chassis_interfaces__srv__GetMapPng_Response__Sequence
{
  chassis_interfaces__srv__GetMapPng_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetMapPng_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__GET_MAP_PNG__STRUCT_H_
