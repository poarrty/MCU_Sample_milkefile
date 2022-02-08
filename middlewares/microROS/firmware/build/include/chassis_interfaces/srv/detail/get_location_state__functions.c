// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:srv/GetLocationState.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/srv/detail/get_location_state__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool
chassis_interfaces__srv__GetLocationState_Request__init(chassis_interfaces__srv__GetLocationState_Request * msg)
{
  if (!msg) {
    return false;
  }
  // structure_needs_at_least_one_member
  return true;
}

void
chassis_interfaces__srv__GetLocationState_Request__fini(chassis_interfaces__srv__GetLocationState_Request * msg)
{
  if (!msg) {
    return;
  }
  // structure_needs_at_least_one_member
}

chassis_interfaces__srv__GetLocationState_Request *
chassis_interfaces__srv__GetLocationState_Request__create()
{
  chassis_interfaces__srv__GetLocationState_Request * msg = (chassis_interfaces__srv__GetLocationState_Request *)malloc(sizeof(chassis_interfaces__srv__GetLocationState_Request));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__srv__GetLocationState_Request));
  bool success = chassis_interfaces__srv__GetLocationState_Request__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__srv__GetLocationState_Request__destroy(chassis_interfaces__srv__GetLocationState_Request * msg)
{
  if (msg) {
    chassis_interfaces__srv__GetLocationState_Request__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__srv__GetLocationState_Request__Sequence__init(chassis_interfaces__srv__GetLocationState_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__srv__GetLocationState_Request * data = NULL;
  if (size) {
    data = (chassis_interfaces__srv__GetLocationState_Request *)calloc(size, sizeof(chassis_interfaces__srv__GetLocationState_Request));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__srv__GetLocationState_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__srv__GetLocationState_Request__fini(&data[i - 1]);
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
chassis_interfaces__srv__GetLocationState_Request__Sequence__fini(chassis_interfaces__srv__GetLocationState_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__srv__GetLocationState_Request__fini(&array->data[i]);
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

chassis_interfaces__srv__GetLocationState_Request__Sequence *
chassis_interfaces__srv__GetLocationState_Request__Sequence__create(size_t size)
{
  chassis_interfaces__srv__GetLocationState_Request__Sequence * array = (chassis_interfaces__srv__GetLocationState_Request__Sequence *)malloc(sizeof(chassis_interfaces__srv__GetLocationState_Request__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__srv__GetLocationState_Request__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__srv__GetLocationState_Request__Sequence__destroy(chassis_interfaces__srv__GetLocationState_Request__Sequence * array)
{
  if (array) {
    chassis_interfaces__srv__GetLocationState_Request__Sequence__fini(array);
  }
  free(array);
}


// Include directives for member types
// Member `msg`
// Member `state`
#include "rosidl_runtime_c/string_functions.h"
// Member `coordinate`
#include "chassis_interfaces/msg/detail/point_coordinate__functions.h"

bool
chassis_interfaces__srv__GetLocationState_Response__init(chassis_interfaces__srv__GetLocationState_Response * msg)
{
  if (!msg) {
    return false;
  }
  // code
  // msg
  if (!rosidl_runtime_c__String__init(&msg->msg)) {
    chassis_interfaces__srv__GetLocationState_Response__fini(msg);
    return false;
  }
  // state
  if (!rosidl_runtime_c__String__init(&msg->state)) {
    chassis_interfaces__srv__GetLocationState_Response__fini(msg);
    return false;
  }
  // coordinate
  if (!chassis_interfaces__msg__PointCoordinate__init(&msg->coordinate)) {
    chassis_interfaces__srv__GetLocationState_Response__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__srv__GetLocationState_Response__fini(chassis_interfaces__srv__GetLocationState_Response * msg)
{
  if (!msg) {
    return;
  }
  // code
  // msg
  rosidl_runtime_c__String__fini(&msg->msg);
  // state
  rosidl_runtime_c__String__fini(&msg->state);
  // coordinate
  chassis_interfaces__msg__PointCoordinate__fini(&msg->coordinate);
}

chassis_interfaces__srv__GetLocationState_Response *
chassis_interfaces__srv__GetLocationState_Response__create()
{
  chassis_interfaces__srv__GetLocationState_Response * msg = (chassis_interfaces__srv__GetLocationState_Response *)malloc(sizeof(chassis_interfaces__srv__GetLocationState_Response));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__srv__GetLocationState_Response));
  bool success = chassis_interfaces__srv__GetLocationState_Response__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__srv__GetLocationState_Response__destroy(chassis_interfaces__srv__GetLocationState_Response * msg)
{
  if (msg) {
    chassis_interfaces__srv__GetLocationState_Response__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__srv__GetLocationState_Response__Sequence__init(chassis_interfaces__srv__GetLocationState_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__srv__GetLocationState_Response * data = NULL;
  if (size) {
    data = (chassis_interfaces__srv__GetLocationState_Response *)calloc(size, sizeof(chassis_interfaces__srv__GetLocationState_Response));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__srv__GetLocationState_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__srv__GetLocationState_Response__fini(&data[i - 1]);
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
chassis_interfaces__srv__GetLocationState_Response__Sequence__fini(chassis_interfaces__srv__GetLocationState_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__srv__GetLocationState_Response__fini(&array->data[i]);
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

chassis_interfaces__srv__GetLocationState_Response__Sequence *
chassis_interfaces__srv__GetLocationState_Response__Sequence__create(size_t size)
{
  chassis_interfaces__srv__GetLocationState_Response__Sequence * array = (chassis_interfaces__srv__GetLocationState_Response__Sequence *)malloc(sizeof(chassis_interfaces__srv__GetLocationState_Response__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__srv__GetLocationState_Response__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__srv__GetLocationState_Response__Sequence__destroy(chassis_interfaces__srv__GetLocationState_Response__Sequence * array)
{
  if (array) {
    chassis_interfaces__srv__GetLocationState_Response__Sequence__fini(array);
  }
  free(array);
}
