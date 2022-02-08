// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/BindGroup.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__BIND_GROUP__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__BIND_GROUP__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'org_code'
// Member 'device_name'
#include "rosidl_runtime_c/string.h"

// Struct defined in srv/BindGroup in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__BindGroup_Request
{
  rosidl_runtime_c__String org_code;
  rosidl_runtime_c__String device_name;
} chassis_interfaces__srv__BindGroup_Request;

// Struct for a sequence of chassis_interfaces__srv__BindGroup_Request.
typedef struct chassis_interfaces__srv__BindGroup_Request__Sequence
{
  chassis_interfaces__srv__BindGroup_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__BindGroup_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'msg'
// already included above
// #include "rosidl_runtime_c/string.h"

// Struct defined in srv/BindGroup in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__BindGroup_Response
{
  uint16_t code;
  rosidl_runtime_c__String msg;
} chassis_interfaces__srv__BindGroup_Response;

// Struct for a sequence of chassis_interfaces__srv__BindGroup_Response.
typedef struct chassis_interfaces__srv__BindGroup_Response__Sequence
{
  chassis_interfaces__srv__BindGroup_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__BindGroup_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__BIND_GROUP__STRUCT_H_
