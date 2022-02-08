// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/Common.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__COMMON__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__COMMON__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in srv/Common in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__Common_Request
{
  uint8_t structure_needs_at_least_one_member;
} chassis_interfaces__srv__Common_Request;

// Struct for a sequence of chassis_interfaces__srv__Common_Request.
typedef struct chassis_interfaces__srv__Common_Request__Sequence
{
  chassis_interfaces__srv__Common_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__Common_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'msg'
#include "rosidl_runtime_c/string.h"

// Struct defined in srv/Common in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__Common_Response
{
  uint16_t code;
  rosidl_runtime_c__String msg;
} chassis_interfaces__srv__Common_Response;

// Struct for a sequence of chassis_interfaces__srv__Common_Response.
typedef struct chassis_interfaces__srv__Common_Response__Sequence
{
  chassis_interfaces__srv__Common_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__Common_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__COMMON__STRUCT_H_
