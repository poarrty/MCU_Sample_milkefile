// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/GetUpdatingInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__GET_UPDATING_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__GET_UPDATING_INFO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in srv/GetUpdatingInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetUpdatingInfo_Request
{
  uint8_t structure_needs_at_least_one_member;
} chassis_interfaces__srv__GetUpdatingInfo_Request;

// Struct for a sequence of chassis_interfaces__srv__GetUpdatingInfo_Request.
typedef struct chassis_interfaces__srv__GetUpdatingInfo_Request__Sequence
{
  chassis_interfaces__srv__GetUpdatingInfo_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetUpdatingInfo_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'total_md5'
// Member 'buffered_md5'
#include "rosidl_runtime_c/string.h"

// Struct defined in srv/GetUpdatingInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetUpdatingInfo_Response
{
  rosidl_runtime_c__String total_md5;
  uint32_t buffered_size;
  rosidl_runtime_c__String buffered_md5;
} chassis_interfaces__srv__GetUpdatingInfo_Response;

// Struct for a sequence of chassis_interfaces__srv__GetUpdatingInfo_Response.
typedef struct chassis_interfaces__srv__GetUpdatingInfo_Response__Sequence
{
  chassis_interfaces__srv__GetUpdatingInfo_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetUpdatingInfo_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__GET_UPDATING_INFO__STRUCT_H_
