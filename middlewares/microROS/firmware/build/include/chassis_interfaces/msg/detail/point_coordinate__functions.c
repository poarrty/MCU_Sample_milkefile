// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/PointCoordinate.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/point_coordinate__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


bool
chassis_interfaces__msg__PointCoordinate__init(chassis_interfaces__msg__PointCoordinate * msg)
{
  if (!msg) {
    return false;
  }
  // x
  // y
  // z
  // yaw
  return true;
}

void
chassis_interfaces__msg__PointCoordinate__fini(chassis_interfaces__msg__PointCoordinate * msg)
{
  if (!msg) {
    return;
  }
  // x
  // y
  // z
  // yaw
}

chassis_interfaces__msg__PointCoordinate *
chassis_interfaces__msg__PointCoordinate__create()
{
  chassis_interfaces__msg__PointCoordinate * msg = (chassis_interfaces__msg__PointCoordinate *)malloc(sizeof(chassis_interfaces__msg__PointCoordinate));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__PointCoordinate));
  bool success = chassis_interfaces__msg__PointCoordinate__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__PointCoordinate__destroy(chassis_interfaces__msg__PointCoordinate * msg)
{
  if (msg) {
    chassis_interfaces__msg__PointCoordinate__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__PointCoordinate__Sequence__init(chassis_interfaces__msg__PointCoordinate__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__PointCoordinate * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__PointCoordinate *)calloc(size, sizeof(chassis_interfaces__msg__PointCoordinate));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__PointCoordinate__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__PointCoordinate__fini(&data[i - 1]);
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
chassis_interfaces__msg__PointCoordinate__Sequence__fini(chassis_interfaces__msg__PointCoordinate__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__PointCoordinate__fini(&array->data[i]);
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

chassis_interfaces__msg__PointCoordinate__Sequence *
chassis_interfaces__msg__PointCoordinate__Sequence__create(size_t size)
{
  chassis_interfaces__msg__PointCoordinate__Sequence * array = (chassis_interfaces__msg__PointCoordinate__Sequence *)malloc(sizeof(chassis_interfaces__msg__PointCoordinate__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__PointCoordinate__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__PointCoordinate__Sequence__destroy(chassis_interfaces__msg__PointCoordinate__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__PointCoordinate__Sequence__fini(array);
  }
  free(array);
}
