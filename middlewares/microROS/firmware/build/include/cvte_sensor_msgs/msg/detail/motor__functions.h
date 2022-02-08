// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from cvte_sensor_msgs:msg/Motor.idl
// generated code does not contain a copyright notice

#ifndef CVTE_SENSOR_MSGS__MSG__DETAIL__MOTOR__FUNCTIONS_H_
#define CVTE_SENSOR_MSGS__MSG__DETAIL__MOTOR__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "cvte_sensor_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "cvte_sensor_msgs/msg/detail/motor__struct.h"

/// Initialize msg/Motor message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * cvte_sensor_msgs__msg__Motor
 * )) before or use
 * cvte_sensor_msgs__msg__Motor__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
bool
cvte_sensor_msgs__msg__Motor__init(cvte_sensor_msgs__msg__Motor * msg);

/// Finalize msg/Motor message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
void
cvte_sensor_msgs__msg__Motor__fini(cvte_sensor_msgs__msg__Motor * msg);

/// Create msg/Motor message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * cvte_sensor_msgs__msg__Motor__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
cvte_sensor_msgs__msg__Motor *
cvte_sensor_msgs__msg__Motor__create();

/// Destroy msg/Motor message.
/**
 * It calls
 * cvte_sensor_msgs__msg__Motor__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
void
cvte_sensor_msgs__msg__Motor__destroy(cvte_sensor_msgs__msg__Motor * msg);


/// Initialize array of msg/Motor messages.
/**
 * It allocates the memory for the number of elements and calls
 * cvte_sensor_msgs__msg__Motor__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
bool
cvte_sensor_msgs__msg__Motor__Sequence__init(cvte_sensor_msgs__msg__Motor__Sequence * array, size_t size);

/// Finalize array of msg/Motor messages.
/**
 * It calls
 * cvte_sensor_msgs__msg__Motor__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
void
cvte_sensor_msgs__msg__Motor__Sequence__fini(cvte_sensor_msgs__msg__Motor__Sequence * array);

/// Create array of msg/Motor messages.
/**
 * It allocates the memory for the array and calls
 * cvte_sensor_msgs__msg__Motor__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
cvte_sensor_msgs__msg__Motor__Sequence *
cvte_sensor_msgs__msg__Motor__Sequence__create(size_t size);

/// Destroy array of msg/Motor messages.
/**
 * It calls
 * cvte_sensor_msgs__msg__Motor__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_cvte_sensor_msgs
void
cvte_sensor_msgs__msg__Motor__Sequence__destroy(cvte_sensor_msgs__msg__Motor__Sequence * array);

#ifdef __cplusplus
}
#endif

#endif  // CVTE_SENSOR_MSGS__MSG__DETAIL__MOTOR__FUNCTIONS_H_
