// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/Task.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/task__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `task_type`
// Member `sub_execute_id`
// Member `map_id_list`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__Task__init(chassis_interfaces__msg__Task * msg)
{
  if (!msg) {
    return false;
  }
  // task_type
  if (!rosidl_runtime_c__String__init(&msg->task_type)) {
    chassis_interfaces__msg__Task__fini(msg);
    return false;
  }
  // sub_execute_id
  if (!rosidl_runtime_c__String__init(&msg->sub_execute_id)) {
    chassis_interfaces__msg__Task__fini(msg);
    return false;
  }
  // map_id_list
  if (!rosidl_runtime_c__String__Sequence__init(&msg->map_id_list, 0)) {
    chassis_interfaces__msg__Task__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__Task__fini(chassis_interfaces__msg__Task * msg)
{
  if (!msg) {
    return;
  }
  // task_type
  rosidl_runtime_c__String__fini(&msg->task_type);
  // sub_execute_id
  rosidl_runtime_c__String__fini(&msg->sub_execute_id);
  // map_id_list
  rosidl_runtime_c__String__Sequence__fini(&msg->map_id_list);
}

chassis_interfaces__msg__Task *
chassis_interfaces__msg__Task__create()
{
  chassis_interfaces__msg__Task * msg = (chassis_interfaces__msg__Task *)malloc(sizeof(chassis_interfaces__msg__Task));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__Task));
  bool success = chassis_interfaces__msg__Task__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__Task__destroy(chassis_interfaces__msg__Task * msg)
{
  if (msg) {
    chassis_interfaces__msg__Task__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__Task__Sequence__init(chassis_interfaces__msg__Task__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__Task * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__Task *)calloc(size, sizeof(chassis_interfaces__msg__Task));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__Task__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__Task__fini(&data[i - 1]);
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
chassis_interfaces__msg__Task__Sequence__fini(chassis_interfaces__msg__Task__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__Task__fini(&array->data[i]);
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

chassis_interfaces__msg__Task__Sequence *
chassis_interfaces__msg__Task__Sequence__create(size_t size)
{
  chassis_interfaces__msg__Task__Sequence * array = (chassis_interfaces__msg__Task__Sequence *)malloc(sizeof(chassis_interfaces__msg__Task__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__Task__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__Task__Sequence__destroy(chassis_interfaces__msg__Task__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__Task__Sequence__fini(array);
  }
  free(array);
}
