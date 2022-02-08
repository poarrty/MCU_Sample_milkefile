// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/DeviceLocation.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/device_location__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `map_id`
// Member `state_time`
#include "rosidl_runtime_c/string_functions.h"
// Member `point`
#include "chassis_interfaces/msg/detail/point_coordinate__functions.h"

bool
chassis_interfaces__msg__DeviceLocation__init(chassis_interfaces__msg__DeviceLocation * msg)
{
  if (!msg) {
    return false;
  }
  // map_id
  if (!rosidl_runtime_c__String__init(&msg->map_id)) {
    chassis_interfaces__msg__DeviceLocation__fini(msg);
    return false;
  }
  // state_time
  if (!rosidl_runtime_c__String__init(&msg->state_time)) {
    chassis_interfaces__msg__DeviceLocation__fini(msg);
    return false;
  }
  // point
  if (!chassis_interfaces__msg__PointCoordinate__init(&msg->point)) {
    chassis_interfaces__msg__DeviceLocation__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__DeviceLocation__fini(chassis_interfaces__msg__DeviceLocation * msg)
{
  if (!msg) {
    return;
  }
  // map_id
  rosidl_runtime_c__String__fini(&msg->map_id);
  // state_time
  rosidl_runtime_c__String__fini(&msg->state_time);
  // point
  chassis_interfaces__msg__PointCoordinate__fini(&msg->point);
}

chassis_interfaces__msg__DeviceLocation *
chassis_interfaces__msg__DeviceLocation__create()
{
  chassis_interfaces__msg__DeviceLocation * msg = (chassis_interfaces__msg__DeviceLocation *)malloc(sizeof(chassis_interfaces__msg__DeviceLocation));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__DeviceLocation));
  bool success = chassis_interfaces__msg__DeviceLocation__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__DeviceLocation__destroy(chassis_interfaces__msg__DeviceLocation * msg)
{
  if (msg) {
    chassis_interfaces__msg__DeviceLocation__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__DeviceLocation__Sequence__init(chassis_interfaces__msg__DeviceLocation__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__DeviceLocation * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__DeviceLocation *)calloc(size, sizeof(chassis_interfaces__msg__DeviceLocation));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__DeviceLocation__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__DeviceLocation__fini(&data[i - 1]);
      }
      free(data);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
chassis_interfaces__msg__DeviceLocation__Sequence__fini(chassis_interfaces__msg__DeviceLocation__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__DeviceLocation__fini(&array->data[i]);
    }
    free(array->data);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

chassis_interfaces__msg__DeviceLocation__Sequence *
chassis_interfaces__msg__DeviceLocation__Sequence__create(size_t size)
{
  chassis_interfaces__msg__DeviceLocation__Sequence * array = (chassis_interfaces__msg__DeviceLocation__Sequence *)malloc(sizeof(chassis_interfaces__msg__DeviceLocation__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__DeviceLocation__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__DeviceLocation__Sequence__destroy(chassis_interfaces__msg__DeviceLocation__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__DeviceLocation__Sequence__fini(array);
  }
  free(array);
}
