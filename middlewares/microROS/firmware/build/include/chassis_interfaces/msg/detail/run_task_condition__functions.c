// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/RunTaskCondition.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/run_task_condition__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `condition_type`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__RunTaskCondition__init(chassis_interfaces__msg__RunTaskCondition * msg)
{
  if (!msg) {
    return false;
  }
  // condition_type
  if (!rosidl_runtime_c__String__init(&msg->condition_type)) {
    chassis_interfaces__msg__RunTaskCondition__fini(msg);
    return false;
  }
  // condition_result
  return true;
}

void
chassis_interfaces__msg__RunTaskCondition__fini(chassis_interfaces__msg__RunTaskCondition * msg)
{
  if (!msg) {
    return;
  }
  // condition_type
  rosidl_runtime_c__String__fini(&msg->condition_type);
  // condition_result
}

chassis_interfaces__msg__RunTaskCondition *
chassis_interfaces__msg__RunTaskCondition__create()
{
  chassis_interfaces__msg__RunTaskCondition * msg = (chassis_interfaces__msg__RunTaskCondition *)malloc(sizeof(chassis_interfaces__msg__RunTaskCondition));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__RunTaskCondition));
  bool success = chassis_interfaces__msg__RunTaskCondition__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__RunTaskCondition__destroy(chassis_interfaces__msg__RunTaskCondition * msg)
{
  if (msg) {
    chassis_interfaces__msg__RunTaskCondition__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__RunTaskCondition__Sequence__init(chassis_interfaces__msg__RunTaskCondition__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__RunTaskCondition * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__RunTaskCondition *)calloc(size, sizeof(chassis_interfaces__msg__RunTaskCondition));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__RunTaskCondition__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__RunTaskCondition__fini(&data[i - 1]);
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
chassis_interfaces__msg__RunTaskCondition__Sequence__fini(chassis_interfaces__msg__RunTaskCondition__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__RunTaskCondition__fini(&array->data[i]);
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

chassis_interfaces__msg__RunTaskCondition__Sequence *
chassis_interfaces__msg__RunTaskCondition__Sequence__create(size_t size)
{
  chassis_interfaces__msg__RunTaskCondition__Sequence * array = (chassis_interfaces__msg__RunTaskCondition__Sequence *)malloc(sizeof(chassis_interfaces__msg__RunTaskCondition__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__RunTaskCondition__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__RunTaskCondition__Sequence__destroy(chassis_interfaces__msg__RunTaskCondition__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__RunTaskCondition__Sequence__fini(array);
  }
  free(array);
}
