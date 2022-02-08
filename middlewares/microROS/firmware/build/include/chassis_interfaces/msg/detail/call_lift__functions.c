// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/CallLift.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/call_lift__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `map_id`
// Member `target_map_id`
// Member `lift_id_list`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__CallLift__init(chassis_interfaces__msg__CallLift * msg)
{
  if (!msg) {
    return false;
  }
  // map_id
  if (!rosidl_runtime_c__String__init(&msg->map_id)) {
    chassis_interfaces__msg__CallLift__fini(msg);
    return false;
  }
  // target_map_id
  if (!rosidl_runtime_c__String__init(&msg->target_map_id)) {
    chassis_interfaces__msg__CallLift__fini(msg);
    return false;
  }
  // lift_id_list
  if (!rosidl_runtime_c__String__Sequence__init(&msg->lift_id_list, 0)) {
    chassis_interfaces__msg__CallLift__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__CallLift__fini(chassis_interfaces__msg__CallLift * msg)
{
  if (!msg) {
    return;
  }
  // map_id
  rosidl_runtime_c__String__fini(&msg->map_id);
  // target_map_id
  rosidl_runtime_c__String__fini(&msg->target_map_id);
  // lift_id_list
  rosidl_runtime_c__String__Sequence__fini(&msg->lift_id_list);
}

chassis_interfaces__msg__CallLift *
chassis_interfaces__msg__CallLift__create()
{
  chassis_interfaces__msg__CallLift * msg = (chassis_interfaces__msg__CallLift *)malloc(sizeof(chassis_interfaces__msg__CallLift));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__CallLift));
  bool success = chassis_interfaces__msg__CallLift__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__CallLift__destroy(chassis_interfaces__msg__CallLift * msg)
{
  if (msg) {
    chassis_interfaces__msg__CallLift__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__CallLift__Sequence__init(chassis_interfaces__msg__CallLift__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__CallLift * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__CallLift *)calloc(size, sizeof(chassis_interfaces__msg__CallLift));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__CallLift__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__CallLift__fini(&data[i - 1]);
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
chassis_interfaces__msg__CallLift__Sequence__fini(chassis_interfaces__msg__CallLift__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__CallLift__fini(&array->data[i]);
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

chassis_interfaces__msg__CallLift__Sequence *
chassis_interfaces__msg__CallLift__Sequence__create(size_t size)
{
  chassis_interfaces__msg__CallLift__Sequence * array = (chassis_interfaces__msg__CallLift__Sequence *)malloc(sizeof(chassis_interfaces__msg__CallLift__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__CallLift__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__CallLift__Sequence__destroy(chassis_interfaces__msg__CallLift__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__CallLift__Sequence__fini(array);
  }
  free(array);
}
