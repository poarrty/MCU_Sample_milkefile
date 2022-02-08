// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/Ledcontrol.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/ledcontrol__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `led_color`
// Member `led_hz`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
chassis_interfaces__msg__Ledcontrol__init(chassis_interfaces__msg__Ledcontrol * msg)
{
  if (!msg) {
    return false;
  }
  // led_color
  if (!rosidl_runtime_c__uint32__Sequence__init(&msg->led_color, 0)) {
    chassis_interfaces__msg__Ledcontrol__fini(msg);
    return false;
  }
  // led_hz
  if (!rosidl_runtime_c__uint8__Sequence__init(&msg->led_hz, 0)) {
    chassis_interfaces__msg__Ledcontrol__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__Ledcontrol__fini(chassis_interfaces__msg__Ledcontrol * msg)
{
  if (!msg) {
    return;
  }
  // led_color
  rosidl_runtime_c__uint32__Sequence__fini(&msg->led_color);
  // led_hz
  rosidl_runtime_c__uint8__Sequence__fini(&msg->led_hz);
}

chassis_interfaces__msg__Ledcontrol *
chassis_interfaces__msg__Ledcontrol__create()
{
  chassis_interfaces__msg__Ledcontrol * msg = (chassis_interfaces__msg__Ledcontrol *)malloc(sizeof(chassis_interfaces__msg__Ledcontrol));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__Ledcontrol));
  bool success = chassis_interfaces__msg__Ledcontrol__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__Ledcontrol__destroy(chassis_interfaces__msg__Ledcontrol * msg)
{
  if (msg) {
    chassis_interfaces__msg__Ledcontrol__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__Ledcontrol__Sequence__init(chassis_interfaces__msg__Ledcontrol__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__Ledcontrol * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__Ledcontrol *)calloc(size, sizeof(chassis_interfaces__msg__Ledcontrol));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__Ledcontrol__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__Ledcontrol__fini(&data[i - 1]);
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
chassis_interfaces__msg__Ledcontrol__Sequence__fini(chassis_interfaces__msg__Ledcontrol__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__Ledcontrol__fini(&array->data[i]);
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

chassis_interfaces__msg__Ledcontrol__Sequence *
chassis_interfaces__msg__Ledcontrol__Sequence__create(size_t size)
{
  chassis_interfaces__msg__Ledcontrol__Sequence * array = (chassis_interfaces__msg__Ledcontrol__Sequence *)malloc(sizeof(chassis_interfaces__msg__Ledcontrol__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__Ledcontrol__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__Ledcontrol__Sequence__destroy(chassis_interfaces__msg__Ledcontrol__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__Ledcontrol__Sequence__fini(array);
  }
  free(array);
}
