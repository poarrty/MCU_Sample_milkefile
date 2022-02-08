// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from chassis_interfaces:msg/UpgradeProgess.idl
// generated code does not contain a copyright notice

#ifndef CHASSIS_INTERFACES__MSG__DETAIL__UPGRADE_PROGESS__STRUCT_H_
#define CHASSIS_INTERFACES__MSG__DETAIL__UPGRADE_PROGESS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Struct defined in msg/UpgradeProgess in the package chassis_interfaces.
typedef struct chassis_interfaces__msg__UpgradeProgess
{
  float progress;
  uint8_t upgradestatus;
} chassis_interfaces__msg__UpgradeProgess;

// Struct for a sequence of chassis_interfaces__msg__UpgradeProgess.
typedef struct chassis_interfaces__msg__UpgradeProgess__Sequence
{
  chassis_interfaces__msg__UpgradeProgess * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} chassis_interfaces__msg__UpgradeProgess__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CHASSIS_INTERFACES__MSG__DETAIL__UPGRADE_PROGESS__STRUCT_H_
