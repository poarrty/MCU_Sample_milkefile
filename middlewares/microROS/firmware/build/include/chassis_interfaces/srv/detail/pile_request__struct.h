// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/PileRequest.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__PILE_REQUEST__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__PILE_REQUEST__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in srv/PileRequest in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__PileRequest_Request
{
  uint16_t pile_id;
} chassis_interfaces__srv__PileRequest_Request;

// Struct for a sequence of chassis_interfaces__srv__PileRequest_Request.
typedef struct chassis_interfaces__srv__PileRequest_Request__Sequence
{
  chassis_interfaces__srv__PileRequest_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__PileRequest_Request__Sequence;


// Constants defined in the message

// Struct defined in srv/PileRequest in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__PileRequest_Response
{
  bool success;
} chassis_interfaces__srv__PileRequest_Response;

// Struct for a sequence of chassis_interfaces__srv__PileRequest_Response.
typedef struct chassis_interfaces__srv__PileRequest_Response__Sequence
{
  chassis_interfaces__srv__PileRequest_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__PileRequest_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__PILE_REQUEST__STRUCT_H_
