// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/MapPointList.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/map_point_list__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `map_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `vec_point`
#include "chassis_interfaces/msg/detail/point_info__functions.h"

bool
chassis_interfaces__msg__MapPointList__init(chassis_interfaces__msg__MapPointList * msg)
{
  if (!msg) {
    return false;
  }
  // map_id
  if (!rosidl_runtime_c__String__init(&msg->map_id)) {
    chassis_interfaces__msg__MapPointList__fini(msg);
    return false;
  }
  // vec_point
  if (!chassis_interfaces__msg__PointInfo__Sequence__init(&msg->vec_point, 0)) {
    chassis_interfaces__msg__MapPointList__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__MapPointList__fini(chassis_interfaces__msg__MapPointList * msg)
{
  if (!msg) {
    return;
  }
  // map_id
  rosidl_runtime_c__String__fini(&msg->map_id);
  // vec_point
  chassis_interfaces__msg__PointInfo__Sequence__fini(&msg->vec_point);
}

chassis_interfaces__msg__MapPointList *
chassis_interfaces__msg__MapPointList__create()
{
  chassis_interfaces__msg__MapPointList * msg = (chassis_interfaces__msg__MapPointList *)malloc(sizeof(chassis_interfaces__msg__MapPointList));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__MapPointList));
  bool success = chassis_interfaces__msg__MapPointList__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__MapPointList__destroy(chassis_interfaces__msg__MapPointList * msg)
{
  if (msg) {
    chassis_interfaces__msg__MapPointList__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__MapPointList__Sequence__init(chassis_interfaces__msg__MapPointList__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__MapPointList * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__MapPointList *)calloc(size, sizeof(chassis_interfaces__msg__MapPointList));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__MapPointList__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__MapPointList__fini(&data[i - 1]);
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
chassis_interfaces__msg__MapPointList__Sequence__fini(chassis_interfaces__msg__MapPointList__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__MapPointList__fini(&array->data[i]);
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

chassis_interfaces__msg__MapPointList__Sequence *
chassis_interfaces__msg__MapPointList__Sequence__create(size_t size)
{
  chassis_interfaces__msg__MapPointList__Sequence * array = (chassis_interfaces__msg__MapPointList__Sequence *)malloc(sizeof(chassis_interfaces__msg__MapPointList__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__MapPointList__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__MapPointList__Sequence__destroy(chassis_interfaces__msg__MapPointList__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__MapPointList__Sequence__fini(array);
  }
  free(array);
}
