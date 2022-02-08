// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/RegionInfo.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/region_info__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `region_id`
// Member `region_type`
#include "rosidl_runtime_c/string_functions.h"
// Member `vec_coordinate`
#include "chassis_interfaces/msg/detail/point_coordinate__functions.h"
// Member `extend_attributes`
#include "chassis_interfaces/msg/detail/string_map__functions.h"

bool
chassis_interfaces__msg__RegionInfo__init(chassis_interfaces__msg__RegionInfo * msg)
{
  if (!msg) {
    return false;
  }
  // region_id
  if (!rosidl_runtime_c__String__init(&msg->region_id)) {
    chassis_interfaces__msg__RegionInfo__fini(msg);
    return false;
  }
  // region_type
  if (!rosidl_runtime_c__String__init(&msg->region_type)) {
    chassis_interfaces__msg__RegionInfo__fini(msg);
    return false;
  }
  // vec_coordinate
  if (!chassis_interfaces__msg__PointCoordinate__Sequence__init(&msg->vec_coordinate, 0)) {
    chassis_interfaces__msg__RegionInfo__fini(msg);
    return false;
  }
  // extend_attributes
  if (!chassis_interfaces__msg__StringMap__Sequence__init(&msg->extend_attributes, 0)) {
    chassis_interfaces__msg__RegionInfo__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__RegionInfo__fini(chassis_interfaces__msg__RegionInfo * msg)
{
  if (!msg) {
    return;
  }
  // region_id
  rosidl_runtime_c__String__fini(&msg->region_id);
  // region_type
  rosidl_runtime_c__String__fini(&msg->region_type);
  // vec_coordinate
  chassis_interfaces__msg__PointCoordinate__Sequence__fini(&msg->vec_coordinate);
  // extend_attributes
  chassis_interfaces__msg__StringMap__Sequence__fini(&msg->extend_attributes);
}

chassis_interfaces__msg__RegionInfo *
chassis_interfaces__msg__RegionInfo__create()
{
  chassis_interfaces__msg__RegionInfo * msg = (chassis_interfaces__msg__RegionInfo *)malloc(sizeof(chassis_interfaces__msg__RegionInfo));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__RegionInfo));
  bool success = chassis_interfaces__msg__RegionInfo__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__RegionInfo__destroy(chassis_interfaces__msg__RegionInfo * msg)
{
  if (msg) {
    chassis_interfaces__msg__RegionInfo__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__RegionInfo__Sequence__init(chassis_interfaces__msg__RegionInfo__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__RegionInfo * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__RegionInfo *)calloc(size, sizeof(chassis_interfaces__msg__RegionInfo));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__RegionInfo__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__RegionInfo__fini(&data[i - 1]);
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
chassis_interfaces__msg__RegionInfo__Sequence__fini(chassis_interfaces__msg__RegionInfo__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__RegionInfo__fini(&array->data[i]);
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

chassis_interfaces__msg__RegionInfo__Sequence *
chassis_interfaces__msg__RegionInfo__Sequence__create(size_t size)
{
  chassis_interfaces__msg__RegionInfo__Sequence * array = (chassis_interfaces__msg__RegionInfo__Sequence *)malloc(sizeof(chassis_interfaces__msg__RegionInfo__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__RegionInfo__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__RegionInfo__Sequence__destroy(chassis_interfaces__msg__RegionInfo__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__RegionInfo__Sequence__fini(array);
  }
  free(array);
}
