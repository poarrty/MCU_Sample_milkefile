// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/MappingPng.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/mapping_png__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `file_md5`
// Member `png_image`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__MappingPng__init(chassis_interfaces__msg__MappingPng * msg)
{
  if (!msg) {
    return false;
  }
  // height
  // width
  // origin_x
  // origin_y
  // resolution
  // file_size
  // file_md5
  if (!rosidl_runtime_c__String__init(&msg->file_md5)) {
    chassis_interfaces__msg__MappingPng__fini(msg);
    return false;
  }
  // png_image
  if (!rosidl_runtime_c__String__init(&msg->png_image)) {
    chassis_interfaces__msg__MappingPng__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__MappingPng__fini(chassis_interfaces__msg__MappingPng * msg)
{
  if (!msg) {
    return;
  }
  // height
  // width
  // origin_x
  // origin_y
  // resolution
  // file_size
  // file_md5
  rosidl_runtime_c__String__fini(&msg->file_md5);
  // png_image
  rosidl_runtime_c__String__fini(&msg->png_image);
}

chassis_interfaces__msg__MappingPng *
chassis_interfaces__msg__MappingPng__create()
{
  chassis_interfaces__msg__MappingPng * msg = (chassis_interfaces__msg__MappingPng *)malloc(sizeof(chassis_interfaces__msg__MappingPng));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__MappingPng));
  bool success = chassis_interfaces__msg__MappingPng__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__MappingPng__destroy(chassis_interfaces__msg__MappingPng * msg)
{
  if (msg) {
    chassis_interfaces__msg__MappingPng__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__MappingPng__Sequence__init(chassis_interfaces__msg__MappingPng__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__MappingPng * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__MappingPng *)calloc(size, sizeof(chassis_interfaces__msg__MappingPng));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__MappingPng__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__MappingPng__fini(&data[i - 1]);
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
chassis_interfaces__msg__MappingPng__Sequence__fini(chassis_interfaces__msg__MappingPng__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__MappingPng__fini(&array->data[i]);
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

chassis_interfaces__msg__MappingPng__Sequence *
chassis_interfaces__msg__MappingPng__Sequence__create(size_t size)
{
  chassis_interfaces__msg__MappingPng__Sequence * array = (chassis_interfaces__msg__MappingPng__Sequence *)malloc(sizeof(chassis_interfaces__msg__MappingPng__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__MappingPng__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__MappingPng__Sequence__destroy(chassis_interfaces__msg__MappingPng__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__MappingPng__Sequence__fini(array);
  }
  free(array);
}
