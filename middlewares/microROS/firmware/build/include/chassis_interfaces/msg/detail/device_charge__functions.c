// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/DeviceCharge.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/device_charge__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `command`
// Member `map_id_list`
// Member `strategy`
// Member `workstation_id`
// Member `charge_trace_id`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__DeviceCharge__init(chassis_interfaces__msg__DeviceCharge * msg)
{
  if (!msg) {
    return false;
  }
  // command
  if (!rosidl_runtime_c__String__init(&msg->command)) {
    chassis_interfaces__msg__DeviceCharge__fini(msg);
    return false;
  }
  // map_id_list
  if (!rosidl_runtime_c__String__Sequence__init(&msg->map_id_list, 0)) {
    chassis_interfaces__msg__DeviceCharge__fini(msg);
    return false;
  }
  // battery
  // strategy
  if (!rosidl_runtime_c__String__init(&msg->strategy)) {
    chassis_interfaces__msg__DeviceCharge__fini(msg);
    return false;
  }
  // workstation_id
  if (!rosidl_runtime_c__String__init(&msg->workstation_id)) {
    chassis_interfaces__msg__DeviceCharge__fini(msg);
    return false;
  }
  // charge_trace_id
  if (!rosidl_runtime_c__String__init(&msg->charge_trace_id)) {
    chassis_interfaces__msg__DeviceCharge__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__DeviceCharge__fini(chassis_interfaces__msg__DeviceCharge * msg)
{
  if (!msg) {
    return;
  }
  // command
  rosidl_runtime_c__String__fini(&msg->command);
  // map_id_list
  rosidl_runtime_c__String__Sequence__fini(&msg->map_id_list);
  // battery
  // strategy
  rosidl_runtime_c__String__fini(&msg->strategy);
  // workstation_id
  rosidl_runtime_c__String__fini(&msg->workstation_id);
  // charge_trace_id
  rosidl_runtime_c__String__fini(&msg->charge_trace_id);
}

chassis_interfaces__msg__DeviceCharge *
chassis_interfaces__msg__DeviceCharge__create()
{
  chassis_interfaces__msg__DeviceCharge * msg = (chassis_interfaces__msg__DeviceCharge *)malloc(sizeof(chassis_interfaces__msg__DeviceCharge));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__DeviceCharge));
  bool success = chassis_interfaces__msg__DeviceCharge__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__DeviceCharge__destroy(chassis_interfaces__msg__DeviceCharge * msg)
{
  if (msg) {
    chassis_interfaces__msg__DeviceCharge__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__DeviceCharge__Sequence__init(chassis_interfaces__msg__DeviceCharge__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__DeviceCharge * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__DeviceCharge *)calloc(size, sizeof(chassis_interfaces__msg__DeviceCharge));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__DeviceCharge__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__DeviceCharge__fini(&data[i - 1]);
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
chassis_interfaces__msg__DeviceCharge__Sequence__fini(chassis_interfaces__msg__DeviceCharge__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__DeviceCharge__fini(&array->data[i]);
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

chassis_interfaces__msg__DeviceCharge__Sequence *
chassis_interfaces__msg__DeviceCharge__Sequence__create(size_t size)
{
  chassis_interfaces__msg__DeviceCharge__Sequence * array = (chassis_interfaces__msg__DeviceCharge__Sequence *)malloc(sizeof(chassis_interfaces__msg__DeviceCharge__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__DeviceCharge__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__DeviceCharge__Sequence__destroy(chassis_interfaces__msg__DeviceCharge__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__DeviceCharge__Sequence__fini(array);
  }
  free(array);
}
