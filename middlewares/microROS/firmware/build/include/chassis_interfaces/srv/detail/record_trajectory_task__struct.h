// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/RecordTrajectoryTask.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__RECORD_TRAJECTORY_TASK__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__RECORD_TRAJECTORY_TASK__STRUCT_H_

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
// Member 'execute_id'
// Member 'execute_report'
// Member 'end_time'
// Member 'end_type'
#include "rosidl_runtime_c/string.h"

// Struct defined in srv/RecordTrajectoryTask in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__RecordTrajectoryTask_Request
{
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String execute_id;
  rosidl_runtime_c__String execute_report;
  rosidl_runtime_c__String end_time;
  rosidl_runtime_c__String end_type;
  uint32_t sequence;
} chassis_interfaces__srv__RecordTrajectoryTask_Request;

// Struct for a sequence of chassis_interfaces__srv__RecordTrajectoryTask_Request.
typedef struct chassis_interfaces__srv__RecordTrajectoryTask_Request__Sequence
{
  chassis_interfaces__srv__RecordTrajectoryTask_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__RecordTrajectoryTask_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'msg'
// already included above
// #include "rosidl_runtime_c/string.h"

// Struct defined in srv/RecordTrajectoryTask in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__RecordTrajectoryTask_Response
{
  uint16_t code;
  rosidl_runtime_c__String msg;
} chassis_interfaces__srv__RecordTrajectoryTask_Response;

// Struct for a sequence of chassis_interfaces__srv__RecordTrajectoryTask_Response.
typedef struct chassis_interfaces__srv__RecordTrajectoryTask_Response__Sequence
{
  chassis_interfaces__srv__RecordTrajectoryTask_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__RecordTrajectoryTask_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__RECORD_TRAJECTORY_TASK__STRUCT_H_
