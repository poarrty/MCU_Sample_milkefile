// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/NaviParam.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/navi_param__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


bool
chassis_interfaces__msg__NaviParam__init(chassis_interfaces__msg__NaviParam * msg)
{
  if (!msg) {
    return false;
  }
  // is_forward
  // is_final_rotate
  // path_type
  // is_abs_reach
  return true;
}

void
chassis_interfaces__msg__NaviParam__fini(chassis_interfaces__msg__NaviParam * msg)
{
  if (!msg) {
    return;
  }
  // is_forward
  // is_final_rotate
  // path_type
  // is_abs_reach
}

chassis_interfaces__msg__NaviParam *
chassis_interfaces__msg__NaviParam__create()
{
  chassis_interfaces__msg__NaviParam * msg = (chassis_interfaces__msg__NaviParam *)malloc(sizeof(chassis_interfaces__msg__NaviParam));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__NaviParam));
  bool success = chassis_interfaces__msg__NaviParam__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__NaviParam__destroy(chassis_interfaces__msg__NaviParam * msg)
{
  if (msg) {
    chassis_interfaces__msg__NaviParam__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__NaviParam__Sequence__init(chassis_interfaces__msg__NaviParam__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__NaviParam * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__NaviParam *)calloc(size, sizeof(chassis_interfaces__msg__NaviParam));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__NaviParam__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__NaviParam__fini(&data[i - 1]);
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
chassis_interfaces__msg__NaviParam__Sequence__fini(chassis_interfaces__msg__NaviParam__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__NaviParam__fini(&array->data[i]);
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

chassis_interfaces__msg__NaviParam__Sequence *
chassis_interfaces__msg__NaviParam__Sequence__create(size_t size)
{
  chassis_interfaces__msg__NaviParam__Sequence * array = (chassis_interfaces__msg__NaviParam__Sequence *)malloc(sizeof(chassis_interfaces__msg__NaviParam__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__NaviParam__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__NaviParam__Sequence__destroy(chassis_interfaces__msg__NaviParam__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__NaviParam__Sequence__fini(array);
  }
  free(array);
}
