// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/WifiInfo.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/wifi_info__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `ssid`
// Member `bssid`
// Member `psk`
// Member `key_mqmt`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__WifiInfo__init(chassis_interfaces__msg__WifiInfo * msg)
{
  if (!msg) {
    return false;
  }
  // ssid
  if (!rosidl_runtime_c__String__init(&msg->ssid)) {
    chassis_interfaces__msg__WifiInfo__fini(msg);
    return false;
  }
  // bssid
  if (!rosidl_runtime_c__String__init(&msg->bssid)) {
    chassis_interfaces__msg__WifiInfo__fini(msg);
    return false;
  }
  // psk
  if (!rosidl_runtime_c__String__init(&msg->psk)) {
    chassis_interfaces__msg__WifiInfo__fini(msg);
    return false;
  }
  // scan_ssid
  // key_mqmt
  if (!rosidl_runtime_c__String__init(&msg->key_mqmt)) {
    chassis_interfaces__msg__WifiInfo__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__WifiInfo__fini(chassis_interfaces__msg__WifiInfo * msg)
{
  if (!msg) {
    return;
  }
  // ssid
  rosidl_runtime_c__String__fini(&msg->ssid);
  // bssid
  rosidl_runtime_c__String__fini(&msg->bssid);
  // psk
  rosidl_runtime_c__String__fini(&msg->psk);
  // scan_ssid
  // key_mqmt
  rosidl_runtime_c__String__fini(&msg->key_mqmt);
}

chassis_interfaces__msg__WifiInfo *
chassis_interfaces__msg__WifiInfo__create()
{
  chassis_interfaces__msg__WifiInfo * msg = (chassis_interfaces__msg__WifiInfo *)malloc(sizeof(chassis_interfaces__msg__WifiInfo));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__WifiInfo));
  bool success = chassis_interfaces__msg__WifiInfo__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__WifiInfo__destroy(chassis_interfaces__msg__WifiInfo * msg)
{
  if (msg) {
    chassis_interfaces__msg__WifiInfo__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__WifiInfo__Sequence__init(chassis_interfaces__msg__WifiInfo__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__WifiInfo * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__WifiInfo *)calloc(size, sizeof(chassis_interfaces__msg__WifiInfo));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__WifiInfo__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__WifiInfo__fini(&data[i - 1]);
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
chassis_interfaces__msg__WifiInfo__Sequence__fini(chassis_interfaces__msg__WifiInfo__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__WifiInfo__fini(&array->data[i]);
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

chassis_interfaces__msg__WifiInfo__Sequence *
chassis_interfaces__msg__WifiInfo__Sequence__create(size_t size)
{
  chassis_interfaces__msg__WifiInfo__Sequence * array = (chassis_interfaces__msg__WifiInfo__Sequence *)malloc(sizeof(chassis_interfaces__msg__WifiInfo__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__WifiInfo__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__WifiInfo__Sequence__destroy(chassis_interfaces__msg__WifiInfo__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__WifiInfo__Sequence__fini(array);
  }
  free(array);
}
