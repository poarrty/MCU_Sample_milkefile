// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/PileGetDevInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__PILE_GET_DEV_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__PILE_GET_DEV_INFO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in srv/PileGetDevInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__PileGetDevInfo_Request
{
  uint8_t structure_needs_at_least_one_member;
} chassis_interfaces__srv__PileGetDevInfo_Request;

// Struct for a sequence of chassis_interfaces__srv__PileGetDevInfo_Request.
typedef struct chassis_interfaces__srv__PileGetDevInfo_Request__Sequence
{
  chassis_interfaces__srv__PileGetDevInfo_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__PileGetDevInfo_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'hardware_id'
// Member 'model_name'
#include "rosidl_runtime_c/string.h"

// Struct defined in srv/PileGetDevInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__PileGetDevInfo_Response
{
  std_msgs__msg__Header header;
  uint64_t iot_id;
  rosidl_runtime_c__String hardware_id;
  rosidl_runtime_c__String model_name;
} chassis_interfaces__srv__PileGetDevInfo_Response;

// Struct for a sequence of chassis_interfaces__srv__PileGetDevInfo_Response.
typedef struct chassis_interfaces__srv__PileGetDevInfo_Response__Sequence
{
  chassis_interfaces__srv__PileGetDevInfo_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__PileGetDevInfo_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__PILE_GET_DEV_INFO__STRUCT_H_
