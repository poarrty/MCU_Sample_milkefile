// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from cvte_sensor_msgs:msg/Motor.idl
// generated code does not contain a copyright notice
#include "cvte_sensor_msgs/msg/detail/motor__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"

bool
cvte_sensor_msgs__msg__Motor__init(cvte_sensor_msgs__msg__Motor * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    cvte_sensor_msgs__msg__Motor__fini(msg);
    return false;
  }
  // realtime_speed_l
  // realtime_speed_r
  return true;
}

void
cvte_sensor_msgs__msg__Motor__fini(cvte_sensor_msgs__msg__Motor * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // realtime_speed_l
  // realtime_speed_r
}

cvte_sensor_msgs__msg__Motor *
cvte_sensor_msgs__msg__Motor__create()
{
  cvte_sensor_msgs__msg__Motor * msg = (cvte_sensor_msgs__msg__Motor *)malloc(sizeof(cvte_sensor_msgs__msg__Motor));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(cvte_sensor_msgs__msg__Motor));
  bool success = cvte_sensor_msgs__msg__Motor__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
cvte_sensor_msgs__msg__Motor__destroy(cvte_sensor_msgs__msg__Motor * msg)
{
  if (msg) {
    cvte_sensor_msgs__msg__Motor__fini(msg);
  }
  free(msg);
}


bool
cvte_sensor_msgs__msg__Motor__Sequence__init(cvte_sensor_msgs__msg__Motor__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  cvte_sensor_msgs__msg__Motor * data = NULL;
  if (size) {
    data = (cvte_sensor_msgs__msg__Motor *)calloc(size, sizeof(cvte_sensor_msgs__msg__Motor));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = cvte_sensor_msgs__msg__Motor__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        cvte_sensor_msgs__msg__Motor__fini(&data[i - 1]);
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
cvte_sensor_msgs__msg__Motor__Sequence__fini(cvte_sensor_msgs__msg__Motor__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      cvte_sensor_msgs__msg__Motor__fini(&array->data[i]);
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

cvte_sensor_msgs__msg__Motor__Sequence *
cvte_sensor_msgs__msg__Motor__Sequence__create(size_t size)
{
  cvte_sensor_msgs__msg__Motor__Sequence * array = (cvte_sensor_msgs__msg__Motor__Sequence *)malloc(sizeof(cvte_sensor_msgs__msg__Motor__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = cvte_sensor_msgs__msg__Motor__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
cvte_sensor_msgs__msg__Motor__Sequence__destroy(cvte_sensor_msgs__msg__Motor__Sequence * array)
{
  if (array) {
    cvte_sensor_msgs__msg__Motor__Sequence__fini(array);
  }
  free(array);
}
