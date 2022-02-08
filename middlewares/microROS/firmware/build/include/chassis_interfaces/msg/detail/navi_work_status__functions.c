// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/NaviWorkStatus.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/navi_work_status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `work_status`
// Member `navigation_state`
// Member `lift_state`
#include "rosidl_runtime_c/string_functions.h"
// Member `navigation_info`
#include "chassis_interfaces/msg/detail/navi_info__functions.h"

bool
chassis_interfaces__msg__NaviWorkStatus__init(chassis_interfaces__msg__NaviWorkStatus * msg)
{
  if (!msg) {
    return false;
  }
  // work_status
  if (!rosidl_runtime_c__String__init(&msg->work_status)) {
    chassis_interfaces__msg__NaviWorkStatus__fini(msg);
    return false;
  }
  // navigation_state
  if (!rosidl_runtime_c__String__init(&msg->navigation_state)) {
    chassis_interfaces__msg__NaviWorkStatus__fini(msg);
    return false;
  }
  // navigation_info
  if (!chassis_interfaces__msg__NaviInfo__init(&msg->navigation_info)) {
    chassis_interfaces__msg__NaviWorkStatus__fini(msg);
    return false;
  }
  // lift_state
  if (!rosidl_runtime_c__String__init(&msg->lift_state)) {
    chassis_interfaces__msg__NaviWorkStatus__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__NaviWorkStatus__fini(chassis_interfaces__msg__NaviWorkStatus * msg)
{
  if (!msg) {
    return;
  }
  // work_status
  rosidl_runtime_c__String__fini(&msg->work_status);
  // navigation_state
  rosidl_runtime_c__String__fini(&msg->navigation_state);
  // navigation_info
  chassis_interfaces__msg__NaviInfo__fini(&msg->navigation_info);
  // lift_state
  rosidl_runtime_c__String__fini(&msg->lift_state);
}

chassis_interfaces__msg__NaviWorkStatus *
chassis_interfaces__msg__NaviWorkStatus__create()
{
  chassis_interfaces__msg__NaviWorkStatus * msg = (chassis_interfaces__msg__NaviWorkStatus *)malloc(sizeof(chassis_interfaces__msg__NaviWorkStatus));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__NaviWorkStatus));
  bool success = chassis_interfaces__msg__NaviWorkStatus__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__NaviWorkStatus__destroy(chassis_interfaces__msg__NaviWorkStatus * msg)
{
  if (msg) {
    chassis_interfaces__msg__NaviWorkStatus__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__NaviWorkStatus__Sequence__init(chassis_interfaces__msg__NaviWorkStatus__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__NaviWorkStatus * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__NaviWorkStatus *)calloc(size, sizeof(chassis_interfaces__msg__NaviWorkStatus));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__NaviWorkStatus__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__NaviWorkStatus__fini(&data[i - 1]);
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
chassis_interfaces__msg__NaviWorkStatus__Sequence__fini(chassis_interfaces__msg__NaviWorkStatus__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__NaviWorkStatus__fini(&array->data[i]);
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

chassis_interfaces__msg__NaviWorkStatus__Sequence *
chassis_interfaces__msg__NaviWorkStatus__Sequence__create(size_t size)
{
  chassis_interfaces__msg__NaviWorkStatus__Sequence * array = (chassis_interfaces__msg__NaviWorkStatus__Sequence *)malloc(sizeof(chassis_interfaces__msg__NaviWorkStatus__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__NaviWorkStatus__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__NaviWorkStatus__Sequence__destroy(chassis_interfaces__msg__NaviWorkStatus__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__NaviWorkStatus__Sequence__fini(array);
  }
  free(array);
}
