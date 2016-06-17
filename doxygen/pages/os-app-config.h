/**
 * @file
 * @brief Application configuration file.
 * @details
 * Many details.
 */

/**
 * @ingroup cmsis-plus-app-config-cmdline
 * @{
 */

/**
 * @brief Enable debug support.
 * @details
 * This definition must always be used in **debug** configurations.
 */
#define DEBUG

/**
 * @brief Enable trace support.
 * @details
 * This definition can be used to enable trace support. Without
 * this definition, all trace calls are inlined to empty statements.
 *
 * It is recommended to use `TRACE` in **debug** configurations.
 */
#define TRACE

/**
 * @brief Disable assert support.
 * @details
 * This is the standard ISO/ANSI definition used to
 * disable all `assert()` statements. Without this definition
 * all `assert()` statements are active, regardless of the presence
 * or absence of the `DEBUG` definition.
 *
 * Assertions are a very valuable mechanism to detect out-of-range
 * conditions, usually in debug configurations, but might add a
 * significant overhead in code size, and a certain overhead in
 * execution time.
 *
 * It is recommended to always use `NDEBUG` in **release**
 * configurations.
 */
#define NDEBUG

/**
 * @} End of ingroup cmsis-plus-app-config-cmdline
 */

// ----------------------------------------------------------------------------

/**
 * @ingroup cmsis-plus-app-config
 * @{
 */

/**
 * @brief Scheduler frequency.
 */
#define OS_INTEGER_SYSTICK_FREQUENCY_HZ (1000)

/**
 * @brief For Cortex-M[347], interrupt priority level.
 */
#define OS_INTEGER_RTOS_CRITICAL_SECTION_INTERRUPT_PRIORITY

#define OS_INTEGER_RTOS_DEFAULT_STACK_SIZE_BYTES
#define OS_INTEGER_RTOS_MAIN_STACK_SIZE_BYTES
#define OS_INTEGER_RTOS_IDLE_STACK_SIZE_BYTES

/**
 * @brief Include statistics to count thread CPU cycles.
 * @details
 * Add support to measure and accumulate accurate thread duration statistics.
 *
 * At each context switch, the high resolution clock is sampled and the
 * difference is added to the current thread statistics variables. At the
 * same time the value is added to a global variable.
 *
 * The RAM overhead of enabling this option is a uint64_t variable for
 * each thread and one global variable.
 *
 * The time overhead is a clock sampling (reading timer registers and
 * possibly adjusting for timer recycle) plus some subtractions/additions
 * on 64-bits.
 *
 * @see os::rtos::scheduler::statistics::cpu_cycles()
 * @see os::rtos::thread::statistics::cpu_cycles()
 *
 * @par Default
 * Disable. Do not include CPU cycles statistics.
 */
#define OS_INCLUDE_RTOS_STATISTICS_THREAD_CPU_CYCLES	(1)

/**
 * @brief Include statistics to count thread context switches.
 * @details
 * Add support to count the number of times each thread was scheduled to run.
 *
 * At each context switch, the thread counter is incremented. At the
 * same time a global counter is also incremented.
 *
 * The RAM overhead of enabling this option is a uint64_t variable for
 * each thread and one global variable.
 *
 * The time overhead is low, incrementing two 64-bit variables.
 *
 * @see os::rtos::scheduler::statistics::context_switches()
 * @see os::rtos::thread::statistics::context_switches()
 *
 * @par Default
 * Disable. Do not include context switches statistics.
 */
#define OS_INCLUDE_RTOS_STATISTICS_THREAD_CONTEXT_SWITCHES

#define OS_INCLUDE_RTOS_CUSTOM_THREAD_USER_STORAGE

#define OS_BOOL_RTOS_MESSAGE_QUEUE_SIZE_16BITS  (true)

#define OS_BOOL_RTOS_THREAD_IDLE_PRIORITY_BELOW_IDLE (true)

#define OS_BOOL_RTOS_PORT_CONTEX_CREATE_ZERO_LR (true)

#define OS_INCLUDE_STANDARD_POSIX_FUNCTIONS
#define OS_INCLUDE_NEWLIB_POSIX_FUNCTIONS

#define OS_INCLUDE_STARTUP_INIT_FP

#define OS_INCLUDE_STARTUP_INIT_MULTIPLE_RAM_SECTIONS

#define OS_INCLUDE_STARTUP_GUARD_CHECKS

#define OS_INTEGER_ATEXIT_ARRAY_SIZE (3)

#define OS_INTEGER_DIRENT_NAME_MAX  (256)

#define OS_EXCLUDE_RTOS_IDLE_WFI

/**
 * @} End of ingroup cmsis-plus-app-config
 */

// ----------------------------------------------------------------------------

/**
 * @ingroup cmsis-plus-app-config-port
 * @{
 */

/**
 * @brief Use a custom scheduler implementation.
 * @details
 * When using a CMSIS++ port that runs on top of another RTOS,
 * this option disables the CMSIS++ reference
 * scheduler and forwards all related calls to the custom
 * implementation RTOS.
 *
 * The default is to use the CMSIS++ reference scheduler.
 */
#define OS_USE_RTOS_PORT_SCHEDULER

/**
 * @brief Use a custom condition variable implementation.
 */
#define OS_USE_RTOS_PORT_CONDITION_VARIABLE
/**
 * @brief Use a custom event flags implementation.
 */
#define OS_USE_RTOS_PORT_EVENT_FLAGS
/**
 * @brief Use a custom message queue implementation.
 */
#define OS_USE_RTOS_PORT_MESSAGE_QUEUE
/**
 * @brief Use a custom mutex implementation.
 */
#define OS_USE_RTOS_PORT_MUTEX
/**
 * @brief Use a custom semaphore implementation.
 */
#define OS_USE_RTOS_PORT_SEMAPHORE
/**
 * @brief Use a custom system tick sleep_for() implementation.
 */
#define OS_USE_RTOS_PORT_SYSTICK_CLOCK_SLEEP_FOR
/**
 * @brief Use a custom real time sleep_for() implementation.
 */
#define OS_USE_RTOS_PORT_REALTIME_CLOCK_SLEEP_FOR
/**
 * @brief Use a custom timer implementation.
 */
#define OS_USE_RTOS_PORT_TIMER

/**
 * @} End of ingroup cmsis-plus-app-config-port
 */


// ----------------------------------------------------------------------------

/**
 * @ingroup cmsis-plus-app-config-trace
 * @{
 */

#define OS_USE_TRACE_ITM
#define OS_INTEGER_TRACE_ITM_STIMULUS_PORT  (0)

#define OS_USE_TRACE_SEMIHOSTING_DEBUG
#define OS_USE_TRACE_SEMIHOSTING_STDOUT
#define OS_INTEGER_TRACE_SEMIHOSTING_TMP_ARRAY_SIZE (16)

#define OS_USE_TRACE_SEGGER_RTT

#define OS_USE_SEMIHOSTING
#define OS_INTEGER_SEMIHOSTING_MAX_OPEN_FILES (20)

#define OS_TRACE_RTOS_CLOCKS
#define OS_TRACE_RTOS_CONDVAR
#define OS_TRACE_RTOS_EVFLAGS
#define OS_TRACE_RTOS_MEMPOOL
#define OS_TRACE_RTOS_MQUEUE
#define OS_TRACE_RTOS_MUTEX
#define OS_TRACE_RTOS_RTC_TICK
#define OS_TRACE_RTOS_SCHEDULER
#define OS_TRACE_RTOS_SEMAPHORE
#define OS_TRACE_RTOS_SYSTICK_TICK
#define OS_TRACE_RTOS_THREAD
#define OS_TRACE_RTOS_THREAD_CONTEXT
#define OS_TRACE_RTOS_THREAD_SIG
#define OS_TRACE_RTOS_TIMER

#define OS_TRACE_RTOS_LISTS

#define OS_TRACE_LIBC_MALLOC
#define OS_TRACE_LIBC_ATEXIT

/**
 * @} End of ingroup cmsis-plus-app-config-trace
 */

// ----------------------------------------------------------------------------

