// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/NaviPointCmd.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/navi_point_cmd__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `map_id`
// Member `point_id`
// Member `point_type`
#include "rosidl_runtime_c/string_functions.h"
// Member `coordinate`
#include "chassis_interfaces/msg/detail/point_coordinate__functions.h"
// Member `navi_param`
#include "chassis_interfaces/msg/detail/navi_param__functions.h"

bool
chassis_interfaces__msg__NaviPointCmd__init(chassis_interfaces__msg__NaviPointCmd * msg)
{
  if (!msg) {
    return false;
  }
  // map_id
  if (!rosidl_runtime_c__String__init(&msg->map_id)) {
    chassis_interfaces__msg__NaviPointCmd__fini(msg);
    return false;
  }
  // point_id
  if (!rosidl_runtime_c__String__init(&msg->point_id)) {
    chassis_interfaces__msg__NaviPointCmd__fini(msg);
    return false;
  }
  // point_type
  if (!rosidl_runtime_c__String__init(&msg->point_type)) {
    chassis_interfaces__msg__NaviPointCmd__fini(msg);
    return false;
  }
  // coordinate
  if (!chassis_interfaces__msg__PointCoordinate__init(&msg->coordinate)) {
    chassis_interfaces__msg__NaviPointCmd__fini(msg);
    return false;
  }
  // navi_param
  if (!chassis_interfaces__msg__NaviParam__init(&msg->navi_param)) {
    chassis_interfaces__msg__NaviPointCmd__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__NaviPointCmd__fini(chassis_interfaces__msg__NaviPointCmd * msg)
{
  if (!msg) {
    return;
  }
  // map_id
  rosidl_runtime_c__String__fini(&msg->map_id);
  // point_id
  rosidl_runtime_c__String__fini(&msg->point_id);
  // point_type
  rosidl_runtime_c__String__fini(&msg->point_type);
  // coordinate
  chassis_interfaces__msg__PointCoordinate__fini(&msg->coordinate);
  // navi_param
  chassis_interfaces__msg__NaviParam__fini(&msg->navi_param);
}

chassis_interfaces__msg__NaviPointCmd *
chassis_interfaces__msg__NaviPointCmd__create()
{
  chassis_interfaces__msg__NaviPointCmd * msg = (chassis_interfaces__msg__NaviPointCmd *)malloc(sizeof(chassis_interfaces__msg__NaviPointCmd));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__NaviPointCmd));
  bool success = chassis_interfaces__msg__NaviPointCmd__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__NaviPointCmd__destroy(chassis_interfaces__msg__NaviPointCmd * msg)
{
  if (msg) {
    chassis_interfaces__msg__NaviPointCmd__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__NaviPointCmd__Sequence__init(chassis_interfaces__msg__NaviPointCmd__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__NaviPointCmd * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__NaviPointCmd *)calloc(size, sizeof(chassis_interfaces__msg__NaviPointCmd));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__NaviPointCmd__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__NaviPointCmd__fini(&data[i - 1]);
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
chassis_interfaces__msg__NaviPointCmd__Sequence__fini(chassis_interfaces__msg__NaviPointCmd__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__NaviPointCmd__fini(&array->data[i]);
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

chassis_interfaces__msg__NaviPointCmd__Sequence *
chassis_interfaces__msg__NaviPointCmd__Sequence__create(size_t size)
{
  chassis_interfaces__msg__NaviPointCmd__Sequence * array = (chassis_interfaces__msg__NaviPointCmd__Sequence *)malloc(sizeof(chassis_interfaces__msg__NaviPointCmd__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__NaviPointCmd__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__NaviPointCmd__Sequence__destroy(chassis_interfaces__msg__NaviPointCmd__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__NaviPointCmd__Sequence__fini(array);
  }
  free(array);
}
