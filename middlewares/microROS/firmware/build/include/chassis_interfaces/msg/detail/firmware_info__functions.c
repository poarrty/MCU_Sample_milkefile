// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from chassis_interfaces:msg/FirmwareInfo.idl
// generated code does not contain a copyright notice
#include "chassis_interfaces/msg/detail/firmware_info__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `app_key`
// Member `version`
// Member `platform`
// Member `upgrade_service_name`
// Member `updating_info_service_name`
// Member `upgrade_progress_topic_name`
#include "rosidl_runtime_c/string_functions.h"

bool
chassis_interfaces__msg__FirmwareInfo__init(chassis_interfaces__msg__FirmwareInfo * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
    return false;
  }
  // priority
  // package_size
  // app_key
  if (!rosidl_runtime_c__String__init(&msg->app_key)) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
    return false;
  }
  // version
  if (!rosidl_runtime_c__String__init(&msg->version)) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
    return false;
  }
  // platform
  if (!rosidl_runtime_c__String__init(&msg->platform)) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
    return false;
  }
  // upgrade_service_name
  if (!rosidl_runtime_c__String__init(&msg->upgrade_service_name)) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
    return false;
  }
  // updating_info_service_name
  if (!rosidl_runtime_c__String__init(&msg->updating_info_service_name)) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
    return false;
  }
  // upgrade_progress_topic_name
  if (!rosidl_runtime_c__String__init(&msg->upgrade_progress_topic_name)) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
    return false;
  }
  return true;
}

void
chassis_interfaces__msg__FirmwareInfo__fini(chassis_interfaces__msg__FirmwareInfo * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // priority
  // package_size
  // app_key
  rosidl_runtime_c__String__fini(&msg->app_key);
  // version
  rosidl_runtime_c__String__fini(&msg->version);
  // platform
  rosidl_runtime_c__String__fini(&msg->platform);
  // upgrade_service_name
  rosidl_runtime_c__String__fini(&msg->upgrade_service_name);
  // updating_info_service_name
  rosidl_runtime_c__String__fini(&msg->updating_info_service_name);
  // upgrade_progress_topic_name
  rosidl_runtime_c__String__fini(&msg->upgrade_progress_topic_name);
}

chassis_interfaces__msg__FirmwareInfo *
chassis_interfaces__msg__FirmwareInfo__create()
{
  chassis_interfaces__msg__FirmwareInfo * msg = (chassis_interfaces__msg__FirmwareInfo *)malloc(sizeof(chassis_interfaces__msg__FirmwareInfo));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(chassis_interfaces__msg__FirmwareInfo));
  bool success = chassis_interfaces__msg__FirmwareInfo__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
chassis_interfaces__msg__FirmwareInfo__destroy(chassis_interfaces__msg__FirmwareInfo * msg)
{
  if (msg) {
    chassis_interfaces__msg__FirmwareInfo__fini(msg);
  }
  free(msg);
}


bool
chassis_interfaces__msg__FirmwareInfo__Sequence__init(chassis_interfaces__msg__FirmwareInfo__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  chassis_interfaces__msg__FirmwareInfo * data = NULL;
  if (size) {
    data = (chassis_interfaces__msg__FirmwareInfo *)calloc(size, sizeof(chassis_interfaces__msg__FirmwareInfo));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = chassis_interfaces__msg__FirmwareInfo__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        chassis_interfaces__msg__FirmwareInfo__fini(&data[i - 1]);
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
chassis_interfaces__msg__FirmwareInfo__Sequence__fini(chassis_interfaces__msg__FirmwareInfo__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      chassis_interfaces__msg__FirmwareInfo__fini(&array->data[i]);
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

chassis_interfaces__msg__FirmwareInfo__Sequence *
chassis_interfaces__msg__FirmwareInfo__Sequence__create(size_t size)
{
  chassis_interfaces__msg__FirmwareInfo__Sequence * array = (chassis_interfaces__msg__FirmwareInfo__Sequence *)malloc(sizeof(chassis_interfaces__msg__FirmwareInfo__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = chassis_interfaces__msg__FirmwareInfo__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
chassis_interfaces__msg__FirmwareInfo__Sequence__destroy(chassis_interfaces__msg__FirmwareInfo__Sequence * array)
{
  if (array) {
    chassis_interfaces__msg__FirmwareInfo__Sequence__fini(array);
  }
  free(array);
}
