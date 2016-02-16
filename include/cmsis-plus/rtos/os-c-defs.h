/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2016 Liviu Ionescu.
 *
 * µOS++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * µOS++ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This file is part of the CMSIS++ proposal, intended as a CMSIS
 * replacement for C++ applications.
 *
 * The structures defined in this file are used both in the C API
 * and in the legacy CMSIS API.
 *
 * Since there is no method to automatically sync them with the C++
 * definitions, they must be manually adjusted to match them, otherwise
 * the validation checks in os-c-wrapper.cpp will fail.
 */

#ifndef CMSIS_PLUS_RTOS_OS_C_STRUCTS_H_
#define CMSIS_PLUS_RTOS_OS_C_STRUCTS_H_

// ----------------------------------------------------------------------------

#include <cmsis-plus/rtos/os-config.h>

#include <stdint.h>
#include <stddef.h>

// ----------------------------------------------------------------------------

#ifdef  __cplusplus
extern "C"
{
#endif

  // --------------------------------------------------------------------------

#define OS_PRIOTHREAD_SIZE_PTR (OS_INTEGER_MAX_NUMBER_OF_THREADS+1)

  // --------------------------------------------------------------------------

  typedef uint32_t os_result_t;

  typedef struct os_thread_list_s
  {
    void* content[OS_PRIOTHREAD_SIZE_PTR];
  } os_thread_list_t;

  // --------------------------------------------------------------------------

  typedef uint8_t os_thread_prio_t;

  enum
  {
    os_priority_idle = 0x01, ///< priority: idle (lowest)
    os_priority_low = 0x40, ///< priority: low
    os_priority_below_normal = 0x60, ///< priority: below normal
    os_priority_normal = 0x80, ///< priority: normal (default)
    os_priority_above_normal = 0xA0, ///< priority: above normal
    os_priority_high = 0xC0, ///< priority: high
    os_priority_realtime = 0xE0, ///< priority: realtime (highest)
    os_priority_error = 0xFE ///< system cannot determine priority or thread has illegal priority
  };

  enum
  {
    os_thread_inactive = 0,
    os_thread_ready,
    os_thread_running,
    os_thread_waiting
  };

  enum
  {
    os_timer_once = 0, os_timer_periodic = 1
  };

  typedef void* os_thread_func_args_t;
  typedef void*
  (*os_thread_func_t) (os_thread_func_args_t args);

  typedef uint8_t os_thread_state_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

  typedef struct os_thread_attr_s
  {
    const char* name;
    void* stack_address;
    size_t stack_size_bytes;
    os_thread_prio_t priority;
  } os_thread_attr_t;

  typedef struct os_thread_s
  {
    const char* name;
    void* stack_addr;
    size_t stack_size_bytes;
    os_thread_func_t func;
    os_thread_func_args_t func_args;
    os_thread_state_t state;
    os_thread_prio_t prio;
    os_result_t wakeup_reason;
  } os_thread_t;

#pragma GCC diagnostic pop

  // --------------------------------------------------------------------------

  typedef void* os_timer_func_args_t;
  typedef void
  (*os_time_func_t) (os_timer_func_args_t args);

  typedef uint8_t os_timer_type_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

  typedef struct os_timer_attr_s
  {
    const char* name;
    os_timer_type_t type;
  } os_timer_attr_t;

  typedef struct os_timer_s
  {
    const char* name;
    os_time_func_t func;
    os_timer_func_args_t func_args;
    os_timer_type_t type;
  } os_timer_t;

#pragma GCC diagnostic pop

  // --------------------------------------------------------------------------

  typedef int16_t os_mutex_count_t;
  typedef uint8_t os_mutex_type_t;
  typedef uint8_t os_mutex_protocol_t;
  typedef uint8_t os_mutex_robustness_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

  typedef struct os_mutex_attr_s
  {
    const char* name;
    os_thread_prio_t priority_ceiling;
    os_mutex_protocol_t protocol;
    os_mutex_robustness_t robustness;
    os_mutex_type_t type;
  } os_mutex_attr_t;

  typedef struct os_mutex_s
  {
    const char* name;
    void* owner;
    os_mutex_count_t count;
    os_thread_prio_t prio_ceiling;
    os_mutex_type_t type;
    os_mutex_protocol_t protocol;
    os_mutex_robustness_t robustness;
  } os_mutex_t;

#pragma GCC diagnostic pop

  // --------------------------------------------------------------------------

  typedef int16_t os_semaphore_count_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

  typedef struct os_semaphore_attr_s
  {
    const char* name;
    os_semaphore_count_t initial_count;
    os_semaphore_count_t max_count;
  } os_semaphore_attr_t;

  typedef struct os_semaphore_s
  {
    const char* name;
    os_thread_list_t list;
    os_semaphore_count_t initial_count;
    os_semaphore_count_t count;
    os_semaphore_count_t max_count;
  } os_semaphore_t;

#pragma GCC diagnostic pop

  // --------------------------------------------------------------------------

  typedef uint16_t os_mempool_size_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

  typedef struct os_mempool_attr_s
  {
    const char* name;
    void* pool_addr;
  } os_mempool_attr_t;

  typedef struct os_mempool_s
  {
    const char* name;
    os_thread_list_t list;
    void* pool_addr;
    os_mempool_size_t blocks;
    os_mempool_size_t block_size_bytes;
    os_mempool_size_t count;
  } os_mempool_t;

#pragma GCC diagnostic pop

  // --------------------------------------------------------------------------

  typedef uint16_t os_mqueue_size_t;
  typedef uint8_t os_mqueue_priority_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

  typedef struct os_mqueue_attr_s
  {
    const char* name;
    void* queue_addr;
    size_t queue_size_bytes;
  } os_mqueue_attr_t;

  typedef struct os_mqueue_s
  {
    const char* name;
    os_thread_list_t send_list;
    os_thread_list_t receive_list;

    void* queue_addr;
    size_t queue_size_bytes;

    os_mqueue_size_t msgs;
    os_mqueue_size_t msg_size_bytes;

    os_mqueue_size_t count;
  } os_mqueue_t;

#pragma GCC diagnostic pop

  // --------------------------------------------------------------------------

  typedef struct os_mail_queue_s
  {
    os_mempool_t pool;
    os_mqueue_t queue;
  } os_mail_queue_t;

// ----------------------------------------------------------------------------

#ifdef  __cplusplus
}
#endif

#endif /* CMSIS_PLUS_RTOS_OS_C_STRUCTS_H_ */