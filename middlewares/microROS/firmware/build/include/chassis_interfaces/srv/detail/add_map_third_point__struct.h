// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:srv/AddMapThirdPoint.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__SRV__DETAIL__ADD_MAP_THIRD_POINT__STRUCT_H_
#define CHASSIS_INTERFACES__SRV__DETAIL__ADD_MAP_THIRD_POINT__STRUCT_H_

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
// Member 'point_type'
#include "rosidl_runtime_c/string.h"
// Member 'coordinate'
#include "chassis_interfaces/msg/detail/point_coordinate__struct.h"
// Member 'extend_attributes'
#include "chassis_interfaces/msg/detail/string_map__struct.h"

// Struct defined in srv/AddMapThirdPoint in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__AddMapThirdPoint_Request
{
  rosidl_runtime_c__String map_id;
  rosidl_runtime_c__String point_type;
  chassis_interfaces__msg__PointCoordinate coordinate;
  chassis_interfaces__msg__StringMap__Sequence extend_attributes;
} chassis_interfaces__srv__AddMapThirdPoint_Request;

// Struct for a sequence of chassis_interfaces__srv__AddMapThirdPoint_Request.
typedef struct chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence
{
  chassis_interfaces__srv__AddMapThirdPoint_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'msg'
// Member 'point_id'
// already included above
// #include "rosidl_runtime_c/string.h"

// Struct defined in srv/AddMapThirdPoint in the package chassis_interfaces.
typedef struct chassis_interfaces__srv__AddMapThirdPoint_Response
{
  uint16_t code;
  rosidl_runtime_c__String msg;
  rosidl_runtime_c__String point_id;
} chassis_interfaces__srv__AddMapThirdPoint_Response;

// Struct for a sequence of chassis_interfaces__srv__AddMapThirdPoint_Response.
typedef struct chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence
{
  chassis_interfaces__srv__AddMapThirdPoint_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__SRV__DETAIL__ADD_MAP_THIRD_POINT__STRUCT_H_
