// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:srv/AddMapThirdPoint.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/srv/detail/add_map_third_point__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Include directives for member types
// Member `map_id`
// Member `point_type`
#include "rosidl_runtime_c/string_functions.h"
// Member `coordinate`
#include "chassis_interfaces/msg/detail/point_coordinate__functions.h"
// Member `extend_attributes`
#include "chassis_interfaces/msg/detail/string_map__functions.h"

bool
chassis_interfaces__srv__AddMapThirdPoint_Request__init(chassis_interfaces__srv__AddMapThirdPoint_Request * msg)
{
  if (!msg) {
    return false;
  }
  // map_id
  if (!rosidl_runtime_c__String__init(&msg->map_id)) {
    chassis_interfaces__srv__AddMapThirdPoint_Request__fini(msg);
    return false;
  }
  // point_type
  if (!rosidl_runtime_c__String__init(&msg->point_type)) {
    chassis_interfaces__srv__AddMapThirdPoint_Request__fini(msg);
    return false;
  }
  // coordinate
  if (!chassis_interfaces__msg__PointCoordinate__init(&msg->coordinate)) {
    chassis_interfaces__srv__AddMapThirdPoint_Request__fini(msg);
    return false;
  }
  // extend_attributes
  if (!chassis_interfaces__msg__StringMap__Sequence__init(&msg->extend_attributes, 0)) {
    chassis_interfaces__srv__AddMapThirdPoint_Request__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__srv__AddMapThirdPoint_Request__fini(chassis_interfaces__srv__AddMapThirdPoint_Request * msg)
{
  if (!msg) {
    return;
  }
  // map_id
  rosidl_runtime_c__String__fini(&msg->map_id);
  // point_type
  rosidl_runtime_c__String__fini(&msg->point_type);
  // coordinate
  chassis_interfaces__msg__PointCoordinate__fini(&msg->coordinate);
  // extend_attributes
  chassis_interfaces__msg__StringMap__Sequence__fini(&msg->extend_attributes);
}

chassis_interfaces__srv__AddMapThirdPoint_Request *
chassis_interfaces__srv__AddMapThirdPoint_Request__create()
{
  chassis_interfaces__srv__AddMapThirdPoint_Request * msg = (chassis_interfaces__srv__AddMapThirdPoint_Request *)malloc(sizeof(chassis_interfaces__srv__AddMapThirdPoint_Request));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__srv__AddMapThirdPoint_Request));
  bool success = chassis_interfaces__srv__AddMapThirdPoint_Request__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__srv__AddMapThirdPoint_Request__destroy(chassis_interfaces__srv__AddMapThirdPoint_Request * msg)
{
  if (msg) {
    chassis_interfaces__srv__AddMapThirdPoint_Request__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence__init(chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__srv__AddMapThirdPoint_Request * data = NULL;
  if (size) {
    data = (chassis_interfaces__srv__AddMapThirdPoint_Request *)calloc(size, sizeof(chassis_interfaces__srv__AddMapThirdPoint_Request));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__srv__AddMapThirdPoint_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__srv__AddMapThirdPoint_Request__fini(&data[i - 1]);
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
chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence__fini(chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__srv__AddMapThirdPoint_Request__fini(&array->data[i]);
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

chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence *
chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence__create(size_t size)
{
  chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence * array = (chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence *)malloc(sizeof(chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence__destroy(chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence * array)
{
  if (array) {
    chassis_interfaces__srv__AddMapThirdPoint_Request__Sequence__fini(array);
  }
  free(array);
}


// Include directives for member types
// Member `msg`
// Member `point_id`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__srv__AddMapThirdPoint_Response__init(chassis_interfaces__srv__AddMapThirdPoint_Response * msg)
{
  if (!msg) {
    return false;
  }
  // code
  // msg
  if (!rosidl_runtime_c__String__init(&msg->msg)) {
    chassis_interfaces__srv__AddMapThirdPoint_Response__fini(msg);
    return false;
  }
  // point_id
  if (!rosidl_runtime_c__String__init(&msg->point_id)) {
    chassis_interfaces__srv__AddMapThirdPoint_Response__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__srv__AddMapThirdPoint_Response__fini(chassis_interfaces__srv__AddMapThirdPoint_Response * msg)
{
  if (!msg) {
    return;
  }
  // code
  // msg
  rosidl_runtime_c__String__fini(&msg->msg);
  // point_id
  rosidl_runtime_c__String__fini(&msg->point_id);
}

chassis_interfaces__srv__AddMapThirdPoint_Response *
chassis_interfaces__srv__AddMapThirdPoint_Response__create()
{
  chassis_interfaces__srv__AddMapThirdPoint_Response * msg = (chassis_interfaces__srv__AddMapThirdPoint_Response *)malloc(sizeof(chassis_interfaces__srv__AddMapThirdPoint_Response));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__srv__AddMapThirdPoint_Response));
  bool success = chassis_interfaces__srv__AddMapThirdPoint_Response__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__srv__AddMapThirdPoint_Response__destroy(chassis_interfaces__srv__AddMapThirdPoint_Response * msg)
{
  if (msg) {
    chassis_interfaces__srv__AddMapThirdPoint_Response__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence__init(chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__srv__AddMapThirdPoint_Response * data = NULL;
  if (size) {
    data = (chassis_interfaces__srv__AddMapThirdPoint_Response *)calloc(size, sizeof(chassis_interfaces__srv__AddMapThirdPoint_Response));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__srv__AddMapThirdPoint_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__srv__AddMapThirdPoint_Response__fini(&data[i - 1]);
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
chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence__fini(chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__srv__AddMapThirdPoint_Response__fini(&array->data[i]);
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

chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence *
chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence__create(size_t size)
{
  chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence * array = (chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence *)malloc(sizeof(chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence__destroy(chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence * array)
{
  if (array) {
    chassis_interfaces__srv__AddMapThirdPoint_Response__Sequence__fini(array);
  }
  free(array);
}
