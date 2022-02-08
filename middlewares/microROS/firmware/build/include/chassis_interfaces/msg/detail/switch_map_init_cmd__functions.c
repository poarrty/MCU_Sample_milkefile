// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/SwitchMapInitCmd.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/switch_map_init_cmd__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `switch_map_id`
// Member `switch_point_id`
// Member `current_point_id`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__SwitchMapInitCmd__init(chassis_interfaces__msg__SwitchMapInitCmd * msg)
{
  if (!msg) {
    return false;
  }
  // switch_map_id
  if (!rosidl_runtime_c__String__init(&msg->switch_map_id)) {
    chassis_interfaces__msg__SwitchMapInitCmd__fini(msg);
    return false;
  }
  // switch_point_id
  if (!rosidl_runtime_c__String__init(&msg->switch_point_id)) {
    chassis_interfaces__msg__SwitchMapInitCmd__fini(msg);
    return false;
  }
  // current_point_id
  if (!rosidl_runtime_c__String__init(&msg->current_point_id)) {
    chassis_interfaces__msg__SwitchMapInitCmd__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__SwitchMapInitCmd__fini(chassis_interfaces__msg__SwitchMapInitCmd * msg)
{
  if (!msg) {
    return;
  }
  // switch_map_id
  rosidl_runtime_c__String__fini(&msg->switch_map_id);
  // switch_point_id
  rosidl_runtime_c__String__fini(&msg->switch_point_id);
  // current_point_id
  rosidl_runtime_c__String__fini(&msg->current_point_id);
}

chassis_interfaces__msg__SwitchMapInitCmd *
chassis_interfaces__msg__SwitchMapInitCmd__create()
{
  chassis_interfaces__msg__SwitchMapInitCmd * msg = (chassis_interfaces__msg__SwitchMapInitCmd *)malloc(sizeof(chassis_interfaces__msg__SwitchMapInitCmd));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__SwitchMapInitCmd));
  bool success = chassis_interfaces__msg__SwitchMapInitCmd__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__SwitchMapInitCmd__destroy(chassis_interfaces__msg__SwitchMapInitCmd * msg)
{
  if (msg) {
    chassis_interfaces__msg__SwitchMapInitCmd__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__SwitchMapInitCmd__Sequence__init(chassis_interfaces__msg__SwitchMapInitCmd__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__SwitchMapInitCmd * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__SwitchMapInitCmd *)calloc(size, sizeof(chassis_interfaces__msg__SwitchMapInitCmd));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__SwitchMapInitCmd__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__SwitchMapInitCmd__fini(&data[i - 1]);
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
chassis_interfaces__msg__SwitchMapInitCmd__Sequence__fini(chassis_interfaces__msg__SwitchMapInitCmd__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__SwitchMapInitCmd__fini(&array->data[i]);
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

chassis_interfaces__msg__SwitchMapInitCmd__Sequence *
chassis_interfaces__msg__SwitchMapInitCmd__Sequence__create(size_t size)
{
  chassis_interfaces__msg__SwitchMapInitCmd__Sequence * array = (chassis_interfaces__msg__SwitchMapInitCmd__Sequence *)malloc(sizeof(chassis_interfaces__msg__SwitchMapInitCmd__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__SwitchMapInitCmd__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__SwitchMapInitCmd__Sequence__destroy(chassis_interfaces__msg__SwitchMapInitCmd__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__SwitchMapInitCmd__Sequence__fini(array);
  }
  free(array);
}
