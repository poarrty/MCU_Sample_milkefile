// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/GetRegionInfo.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__GET_REGION_INFO__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__GET_REGION_INFO__STRUCT_H_

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
// Member 'task_id'
// Member 'region_type'
#include "rosidl_runtime_c/string.h"

// Struct defined in srv/GetRegionInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetRegionInfo_Request
{
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String task_id;
  rosidl_runtime_c__String region_type;
} chassis_interfaces__srv__GetRegionInfo_Request;

// Struct for a sequence of chassis_interfaces__srv__GetRegionInfo_Request.
typedef struct chassis_interfaces__srv__GetRegionInfo_Request__Sequence
{
  chassis_interfaces__srv__GetRegionInfo_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetRegionInfo_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'msg'
// already included above
// #include "rosidl_runtime_c/string.h"
// Member 'vec_region_info'
#include "chassis_interfaces/msg/detail/region_info__struct.h"

// Struct defined in srv/GetRegionInfo in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__GetRegionInfo_Response
{
  uint16_t code;
  rosidl_runtime_c__String msg;
  chassis_interfaces__msg__RegionInfo__Sequence vec_region_info;
} chassis_interfaces__srv__GetRegionInfo_Response;

// Struct for a sequence of chassis_interfaces__srv__GetRegionInfo_Response.
typedef struct chassis_interfaces__srv__GetRegionInfo_Response__Sequence
{
  chassis_interfaces__srv__GetRegionInfo_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__GetRegionInfo_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__GET_REGION_INFO__STRUCT_H_
