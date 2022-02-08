// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/Upgrade.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__UPGRADE__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__UPGRADE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'total_md5'
// Member 'package_md5'
#include "rosidl_runtime_c/string.h"
// Member 'package_data'
#include "rosidl_runtime_c/primitives_sequence.h"

// Struct defined in srv/Upgrade in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__Upgrade_Request
{
  rosidl_runtime_c__String total_md5;
  uint32_t total_size;
  uint32_t package_offset;
  rosidl_runtime_c__String package_md5;
  uint32_t package_size;
  rosidl_runtime_c__uint8__Sequence package_data;
} chassis_interfaces__srv__Upgrade_Request;

// Struct for a sequence of chassis_interfaces__srv__Upgrade_Request.
typedef struct chassis_interfaces__srv__Upgrade_Request__Sequence
{
  chassis_interfaces__srv__Upgrade_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__Upgrade_Request__Sequence;


// Constants defined in the message

// Struct defined in srv/Upgrade in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__Upgrade_Response
{
  bool success;
} chassis_interfaces__srv__Upgrade_Response;

// Struct for a sequence of chassis_interfaces__srv__Upgrade_Response.
typedef struct chassis_interfaces__srv__Upgrade_Response__Sequence
{
  chassis_interfaces__srv__Upgrade_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__Upgrade_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__UPGRADE__STRUCT_H_
