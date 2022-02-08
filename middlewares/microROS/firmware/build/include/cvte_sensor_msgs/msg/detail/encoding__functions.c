// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from cvte_sensor_msgs:msg/Encoding.idl
// generated code does not contain a copyright notice
#include "cvte_sensor_msgs/msg/detail/encoding__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `encoding_num`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
cvte_sensor_msgs__msg__Encoding__init(cvte_sensor_msgs__msg__Encoding * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    cvte_sensor_msgs__msg__Encoding__fini(msg);
    return false;
  }
  // encoding_num
  if (!rosidl_runtime_c__int64__Sequence__init(&msg->encoding_num, 0)) {
    cvte_sensor_msgs__msg__Encoding__fini(msg);
    return false;
  }
  // encoding_circle
  return true;
}

void
cvte_sensor_msgs__msg__Encoding__fini(cvte_sensor_msgs__msg__Encoding * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // encoding_num
  rosidl_runtime_c__int64__Sequence__fini(&msg->encoding_num);
  // encoding_circle
}

cvte_sensor_msgs__msg__Encoding *
cvte_sensor_msgs__msg__Encoding__create()
{
  cvte_sensor_msgs__msg__Encoding * msg = (cvte_sensor_msgs__msg__Encoding *)malloc(sizeof(cvte_sensor_msgs__msg__Encoding));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(cvte_sensor_msgs__msg__Encoding));
  bool success = cvte_sensor_msgs__msg__Encoding__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
cvte_sensor_msgs__msg__Encoding__destroy(cvte_sensor_msgs__msg__Encoding * msg)
{
  if (msg) {
    cvte_sensor_msgs__msg__Encoding__fini(msg);
  }
  free(msg);
}


bool
cvte_sensor_msgs__msg__Encoding__Sequence__init(cvte_sensor_msgs__msg__Encoding__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  cvte_sensor_msgs__msg__Encoding * data = NULL;
  if (size) {
    data = (cvte_sensor_msgs__msg__Encoding *)calloc(size, sizeof(cvte_sensor_msgs__msg__Encoding));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = cvte_sensor_msgs__msg__Encoding__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        cvte_sensor_msgs__msg__Encoding__fini(&data[i - 1]);
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
cvte_sensor_msgs__msg__Encoding__Sequence__fini(cvte_sensor_msgs__msg__Encoding__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      cvte_sensor_msgs__msg__Encoding__fini(&array->data[i]);
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

cvte_sensor_msgs__msg__Encoding__Sequence *
cvte_sensor_msgs__msg__Encoding__Sequence__create(size_t size)
{
  cvte_sensor_msgs__msg__Encoding__Sequence * array = (cvte_sensor_msgs__msg__Encoding__Sequence *)malloc(sizeof(cvte_sensor_msgs__msg__Encoding__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = cvte_sensor_msgs__msg__Encoding__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
cvte_sensor_msgs__msg__Encoding__Sequence__destroy(cvte_sensor_msgs__msg__Encoding__Sequence * array)
{
  if (array) {
    cvte_sensor_msgs__msg__Encoding__Sequence__fini(array);
  }
  free(array);
}
