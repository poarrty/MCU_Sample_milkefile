// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/SelfCheck.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/self_check__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `sub_execute_id`
// Member `check_result`
// Member `check_msg`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__SelfCheck__init(chassis_interfaces__msg__SelfCheck * msg)
{
  if (!msg) {
    return false;
  }
  // sub_execute_id
  if (!rosidl_runtime_c__String__init(&msg->sub_execute_id)) {
    chassis_interfaces__msg__SelfCheck__fini(msg);
    return false;
  }
  // check_result
  if (!rosidl_runtime_c__String__init(&msg->check_result)) {
    chassis_interfaces__msg__SelfCheck__fini(msg);
    return false;
  }
  // check_msg
  if (!rosidl_runtime_c__String__init(&msg->check_msg)) {
    chassis_interfaces__msg__SelfCheck__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__SelfCheck__fini(chassis_interfaces__msg__SelfCheck * msg)
{
  if (!msg) {
    return;
  }
  // sub_execute_id
  rosidl_runtime_c__String__fini(&msg->sub_execute_id);
  // check_result
  rosidl_runtime_c__String__fini(&msg->check_result);
  // check_msg
  rosidl_runtime_c__String__fini(&msg->check_msg);
}

chassis_interfaces__msg__SelfCheck *
chassis_interfaces__msg__SelfCheck__create()
{
  chassis_interfaces__msg__SelfCheck * msg = (chassis_interfaces__msg__SelfCheck *)malloc(sizeof(chassis_interfaces__msg__SelfCheck));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__SelfCheck));
  bool success = chassis_interfaces__msg__SelfCheck__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__SelfCheck__destroy(chassis_interfaces__msg__SelfCheck * msg)
{
  if (msg) {
    chassis_interfaces__msg__SelfCheck__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__SelfCheck__Sequence__init(chassis_interfaces__msg__SelfCheck__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__SelfCheck * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__SelfCheck *)calloc(size, sizeof(chassis_interfaces__msg__SelfCheck));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__SelfCheck__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__SelfCheck__fini(&data[i - 1]);
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
chassis_interfaces__msg__SelfCheck__Sequence__fini(chassis_interfaces__msg__SelfCheck__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__SelfCheck__fini(&array->data[i]);
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

chassis_interfaces__msg__SelfCheck__Sequence *
chassis_interfaces__msg__SelfCheck__Sequence__create(size_t size)
{
  chassis_interfaces__msg__SelfCheck__Sequence * array = (chassis_interfaces__msg__SelfCheck__Sequence *)malloc(sizeof(chassis_interfaces__msg__SelfCheck__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__SelfCheck__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__SelfCheck__Sequence__destroy(chassis_interfaces__msg__SelfCheck__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__SelfCheck__Sequence__fini(array);
  }
  free(array);
}
