// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/GetMapList.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__GET_MAP_LIST__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__GET_MAP_LIST__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in srv/GetMapList in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetMapList_Request
{
  uint8_t structure_needs_at_least_one_member;
} chassis_interfaces__srv__GetMapList_Request;

// Struct for a sequence of chassis_interfaces__srv__GetMapList_Request.
typedef struct chassis_interfaces__srv__GetMapList_Request__Sequence
{
  chassis_interfaces__srv__GetMapList_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetMapList_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'msg'
#include "rosidl_runtime_c/string.h"
// Member 'map_list'
#include "chassis_interfaces/msg/detail/map_data_info__struct.h"

// Struct defined in srv/GetMapList in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetMapList_Response
{
  uint16_t code;
  rosidl_runtime_c__String msg;
  chassis_interfaces__msg__MapDataInfo__Sequence map_list;
} chassis_interfaces__srv__GetMapList_Response;

// Struct for a sequence of chassis_interfaces__srv__GetMapList_Response.
typedef struct chassis_interfaces__srv__GetMapList_Response__Sequence
{
  chassis_interfaces__srv__GetMapList_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetMapList_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__GET_MAP_LIST__STRUCT_H_
