// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/DeviceCmd.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/device_cmd__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `cmd`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__DeviceCmd__init(chassis_interfaces__msg__DeviceCmd * msg)
{
  if (!msg) {
    return false;
  }
  // cmd
  if (!rosidl_runtime_c__String__init(&msg->cmd)) {
    chassis_interfaces__msg__DeviceCmd__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__DeviceCmd__fini(chassis_interfaces__msg__DeviceCmd * msg)
{
  if (!msg) {
    return;
  }
  // cmd
  rosidl_runtime_c__String__fini(&msg->cmd);
}

chassis_interfaces__msg__DeviceCmd *
chassis_interfaces__msg__DeviceCmd__create()
{
  chassis_interfaces__msg__DeviceCmd * msg = (chassis_interfaces__msg__DeviceCmd *)malloc(sizeof(chassis_interfaces__msg__DeviceCmd));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__DeviceCmd));
  bool success = chassis_interfaces__msg__DeviceCmd__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__DeviceCmd__destroy(chassis_interfaces__msg__DeviceCmd * msg)
{
  if (msg) {
    chassis_interfaces__msg__DeviceCmd__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__DeviceCmd__Sequence__init(chassis_interfaces__msg__DeviceCmd__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__DeviceCmd * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__DeviceCmd *)calloc(size, sizeof(chassis_interfaces__msg__DeviceCmd));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__DeviceCmd__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__DeviceCmd__fini(&data[i - 1]);
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
chassis_interfaces__msg__DeviceCmd__Sequence__fini(chassis_interfaces__msg__DeviceCmd__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__DeviceCmd__fini(&array->data[i]);
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

chassis_interfaces__msg__DeviceCmd__Sequence *
chassis_interfaces__msg__DeviceCmd__Sequence__create(size_t size)
{
  chassis_interfaces__msg__DeviceCmd__Sequence * array = (chassis_interfaces__msg__DeviceCmd__Sequence *)malloc(sizeof(chassis_interfaces__msg__DeviceCmd__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__DeviceCmd__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__DeviceCmd__Sequence__destroy(chassis_interfaces__msg__DeviceCmd__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__DeviceCmd__Sequence__fini(array);
  }
  free(array);
}
