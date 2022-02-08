// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/MapWorkStatus.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/map_work_status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `work_status`
// Member `location_state`
// Member `location_operation_state`
// Member `map_id`
// Member `map_create_time`
#include "rosidl_runtime_c/string_functions.h"
// Member `coordinate`
#include "chassis_interfaces/msg/detail/point_coordinate__functions.h"

bool
chassis_interfaces__msg__MapWorkStatus__init(chassis_interfaces__msg__MapWorkStatus * msg)
{
  if (!msg) {
    return false;
  }
  // work_status
  if (!rosidl_runtime_c__String__init(&msg->work_status)) {
    chassis_interfaces__msg__MapWorkStatus__fini(msg);
    return false;
  }
  // location_state
  if (!rosidl_runtime_c__String__init(&msg->location_state)) {
    chassis_interfaces__msg__MapWorkStatus__fini(msg);
    return false;
  }
  // location_operation_state
  if (!rosidl_runtime_c__String__init(&msg->location_operation_state)) {
    chassis_interfaces__msg__MapWorkStatus__fini(msg);
    return false;
  }
  // coordinate
  if (!chassis_interfaces__msg__PointCoordinate__init(&msg->coordinate)) {
    chassis_interfaces__msg__MapWorkStatus__fini(msg);
    return false;
  }
  // map_id
  if (!rosidl_runtime_c__String__init(&msg->map_id)) {
    chassis_interfaces__msg__MapWorkStatus__fini(msg);
    return false;
  }
  // map_create_time
  if (!rosidl_runtime_c__String__init(&msg->map_create_time)) {
    chassis_interfaces__msg__MapWorkStatus__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__MapWorkStatus__fini(chassis_interfaces__msg__MapWorkStatus * msg)
{
  if (!msg) {
    return;
  }
  // work_status
  rosidl_runtime_c__String__fini(&msg->work_status);
  // location_state
  rosidl_runtime_c__String__fini(&msg->location_state);
  // location_operation_state
  rosidl_runtime_c__String__fini(&msg->location_operation_state);
  // coordinate
  chassis_interfaces__msg__PointCoordinate__fini(&msg->coordinate);
  // map_id
  rosidl_runtime_c__String__fini(&msg->map_id);
  // map_create_time
  rosidl_runtime_c__String__fini(&msg->map_create_time);
}

chassis_interfaces__msg__MapWorkStatus *
chassis_interfaces__msg__MapWorkStatus__create()
{
  chassis_interfaces__msg__MapWorkStatus * msg = (chassis_interfaces__msg__MapWorkStatus *)malloc(sizeof(chassis_interfaces__msg__MapWorkStatus));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__MapWorkStatus));
  bool success = chassis_interfaces__msg__MapWorkStatus__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__MapWorkStatus__destroy(chassis_interfaces__msg__MapWorkStatus * msg)
{
  if (msg) {
    chassis_interfaces__msg__MapWorkStatus__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__MapWorkStatus__Sequence__init(chassis_interfaces__msg__MapWorkStatus__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__MapWorkStatus * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__MapWorkStatus *)calloc(size, sizeof(chassis_interfaces__msg__MapWorkStatus));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__MapWorkStatus__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__MapWorkStatus__fini(&data[i - 1]);
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
chassis_interfaces__msg__MapWorkStatus__Sequence__fini(chassis_interfaces__msg__MapWorkStatus__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__MapWorkStatus__fini(&array->data[i]);
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

chassis_interfaces__msg__MapWorkStatus__Sequence *
chassis_interfaces__msg__MapWorkStatus__Sequence__create(size_t size)
{
  chassis_interfaces__msg__MapWorkStatus__Sequence * array = (chassis_interfaces__msg__MapWorkStatus__Sequence *)malloc(sizeof(chassis_interfaces__msg__MapWorkStatus__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__MapWorkStatus__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__MapWorkStatus__Sequence__destroy(chassis_interfaces__msg__MapWorkStatus__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__MapWorkStatus__Sequence__fini(array);
  }
  free(array);
}
