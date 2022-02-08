// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/NaviInfo.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/navi_info__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `map_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `point_info`
#include "chassis_interfaces/msg/detail/navi_point_info__functions.h"
// Member `path_info`
#include "chassis_interfaces/msg/detail/navi_path_info__functions.h"
// Member `region_info`
#include "chassis_interfaces/msg/detail/navi_region_info__functions.h"

bool
chassis_interfaces__msg__NaviInfo__init(chassis_interfaces__msg__NaviInfo * msg)
{
  if (!msg) {
    return false;
  }
  // map_id
  if (!rosidl_runtime_c__String__init(&msg->map_id)) {
    chassis_interfaces__msg__NaviInfo__fini(msg);
    return false;
  }
  // point_info
  if (!chassis_interfaces__msg__NaviPointInfo__init(&msg->point_info)) {
    chassis_interfaces__msg__NaviInfo__fini(msg);
    return false;
  }
  // path_info
  if (!chassis_interfaces__msg__NaviPathInfo__init(&msg->path_info)) {
    chassis_interfaces__msg__NaviInfo__fini(msg);
    return false;
  }
  // region_info
  if (!chassis_interfaces__msg__NaviRegionInfo__init(&msg->region_info)) {
    chassis_interfaces__msg__NaviInfo__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__NaviInfo__fini(chassis_interfaces__msg__NaviInfo * msg)
{
  if (!msg) {
    return;
  }
  // map_id
  rosidl_runtime_c__String__fini(&msg->map_id);
  // point_info
  chassis_interfaces__msg__NaviPointInfo__fini(&msg->point_info);
  // path_info
  chassis_interfaces__msg__NaviPathInfo__fini(&msg->path_info);
  // region_info
  chassis_interfaces__msg__NaviRegionInfo__fini(&msg->region_info);
}

chassis_interfaces__msg__NaviInfo *
chassis_interfaces__msg__NaviInfo__create()
{
  chassis_interfaces__msg__NaviInfo * msg = (chassis_interfaces__msg__NaviInfo *)malloc(sizeof(chassis_interfaces__msg__NaviInfo));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__NaviInfo));
  bool success = chassis_interfaces__msg__NaviInfo__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__NaviInfo__destroy(chassis_interfaces__msg__NaviInfo * msg)
{
  if (msg) {
    chassis_interfaces__msg__NaviInfo__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__NaviInfo__Sequence__init(chassis_interfaces__msg__NaviInfo__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__NaviInfo * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__NaviInfo *)calloc(size, sizeof(chassis_interfaces__msg__NaviInfo));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__NaviInfo__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__NaviInfo__fini(&data[i - 1]);
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
chassis_interfaces__msg__NaviInfo__Sequence__fini(chassis_interfaces__msg__NaviInfo__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__NaviInfo__fini(&array->data[i]);
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

chassis_interfaces__msg__NaviInfo__Sequence *
chassis_interfaces__msg__NaviInfo__Sequence__create(size_t size)
{
  chassis_interfaces__msg__NaviInfo__Sequence * array = (chassis_interfaces__msg__NaviInfo__Sequence *)malloc(sizeof(chassis_interfaces__msg__NaviInfo__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__NaviInfo__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__NaviInfo__Sequence__destroy(chassis_interfaces__msg__NaviInfo__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__NaviInfo__Sequence__fini(array);
  }
  free(array);
}
