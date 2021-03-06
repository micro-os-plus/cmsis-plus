# Criticism & suggestions

These are some issues related to the current CMSIS APIs, with suggestions based on solutions used in µOS++.

## CMSIS RTOS API: Criticism, comments and µOS++ suggestions

### For the impatient

If you ever had to do with CMSIS RTOS API and did not enjoy it, or if you felt it like a straitjacket compared to your native RTOS, well, rest assured, your're not alone. The good news is that your experience matters and you can help improve CMSIS RTOS API. Go to [GitHub Issues](https://github.com/ARM-software/CMSIS/issues/64) and comment the existing issues, or open new ones.



## The long story

### Thumbs up ARM for the CMSIS RTOS idea! 

First of all I have to confess that I was a big supporter of the general idea of a common CMSIS RTOS API, from the moment I first read about it. However, as big as my expectations were, as big was my dissapointment when the specs went out.

### Some CMSIS APIs considerations

From my point of view, the main problems with the CMSIS RTOS API are:

- no POSIX compliance
- not C++ friendly.

Please note that I did not ask for C++ APIs, the plain C APIs should be perfectly fine, I just preferred tha APIs to be designed by someone who thinks in C++, not in C (and as such knows how to avoid the usual mess that unstructured C programs bring, especially in the embedded world); unfortunately ARM seems to have no C++ specialists in their design teams.

### The CMSIS++ proposal

Given this situation, and seeing that ARM had no plans for a C++ redesign, I started to think of [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), as a C++ POSIX compliant proposal for a future generation of CMSIS.
 
### Some CMSIS RTOS API issues

The initial attempt in CMSIS++ was to simply rewrite the original RTOS API in C++. However, while starting to walk this path, I encountered so many problems, and noticed so many differences from the POSIX and ISO C/C++ specs, that at a certain point I realised that the current design is broken beyound repair, and a reset is required, otherwise this approach will never work. 

Restarting from scratch, the focus moved from CMSIS to POSIX and ISO.

During the design and development phases, I kept a log of issues that I identified and addressed in the CMSIS++ proposal.

Some were difficulties in understanding the CMSIS RTOS API, due to documentation issues, some are functional issues that make using the original API not very convenient, and some are suggestions for missing features.


The POSIX compliance issues are:

Use POSIX error codes (#65)
Use explicit separate calls for different waiting functions, like lock(), try\_lock(), timed\_wait() (#45)
Add normal (non-recursive) mutex (#53)
Add a mechanism to wait for a thread to terminate (#50)
For message queues, make the message size user configurable (#70)
For message queues, add message priorities (#72)
Make osSemaphoreWait() return errors, not counts (#56)
Deprecate or remove the unused thread_id parameter in osMessageQCreate()/osMailQCreate() prototypes (#61)

Other functional issues are:

Avoid the heavy use of macros (to define objects and to refer to them) (#36)
Do not mandate the use of a dynamic allocator (for stack, queues, etc) (#37)
Add support for critical regions (interrupts & scheduler) (#38)
Avoid mixing time durations (in milliseconds) with timer counts in ticks (#39)
Add a separate RTC system clock (#40)
Add os_main() to make the use of a main thread explicit (#41)
Add support for a synchronised public memory allocator (#42)
Avoid returning aggregates (like osEvent) (#43)
Extend the range for osKernelSysTick() (#44)
Make the scheme to assign names to objects more consecvent (#46)
Add missing destructor functions to all objects (#47)
Extend the range of priority levels (#49)
Add a mechanism to enumerate all registered threads (#51)
Allow to explicitly define the semaphore max count (#55)
Add a method to wait for a memory pool block to become available (#57)
Fix non-portable message type in osMessagePut() (#60)
Fix osMessagePut()/osMailPut() inconsistent error when called from ISR (#62)
Mail queues, as separate objects, are redundant (#63)
Add typedefs for all different types used in prototypes (#66)
For all objects, add reset functions to return the object to initial status (#67)
For mutex, add a method to get the owner thread (#68)
For memory pools, add more accessors to get pool status (#69)
For message queues, add more accessors to get queue status (#71)


The documentation issues are:

Explain that thread functions can return (#48)
Explain the mutex behaviour (recursive vs normal) (#52)
Clarify the specs for binary vs counting semaphores (#54)
Fix the data type used in osMessageQDef() example (#58)
Fix misplaced thread id parameter for message queue (#59)


### CMSIS RTOS API v2

Somehow acknowledging the initial design problems, ARM announced working on CMSIS RTOS API v2. To my surprise, ARM seems to have deprecated the initial macro based object creation mechanism (probably one of the most annoying features of the RTOS API v1).

ARM also gave up returning aggregate objects, extended the priorities range, added explicit normal/recursive mutex objects, renamed some objects and generally kept very few features from the initial specification, so a design reset seems possible.

However, based on the µOS++ experience, there are still more design decisions required to bring the new RTOS v2 closer to POSIX and ISO, for example using the POSIX error codes, using the POSIX explicit separate calls for different waiting functions (like `lock()`, `try_lock()`, `timed_wait()`), etc.


----


### Final personal thoughts

As a personal remark, the design of the initial CMSIS RTOS API seems greatly influenced by Keil RTX (function names and prototypes), which, in my opinion, might not necessarily be the most fortunate design, with some ideas from POSIX threads, an established standard.

However the expected influence from POSIX threads is mostly apparent and  inconsecvent.

In POSIX threads one of the design characteristics is the use of attributes to configure various creation parameters, instead of passing all of them in a long prototype. As such, all functions used to create objects have a pointer to the attributes. For simple use cases, this pointer can be NULL, and reasonable defaults are applied. After this pointer, the function prototypes include a minimum set of mandatory parameters (for example a pointer to the thread function when creating threads).

This design pattern allows to clearly separate simple use cases from advanced ones.

The CMSIS RTOS API design introduces `*Def` structures, and a pointer to them is passed to the creation functions, but this pointer is mandatory, it cannot be NULL, and there is not distinction between simple and advanced 
use cases.

Even worse, not all creation parameters are available, neither in the `*Def` structure nor in the `*Create()` prototype (for example the location of a user supplied thread stack area, the semaphore max count, etc). 

Also the split between the parameters that go into the `*Def` structure  and those that go to the `*Create()` prototype is not obvious and consecvent (as it is in POSIX threads).

The CMSIS++ proposal takes care of all these details, bringing the CMSIS++ RTOS API much closer to POSIX and ISO C/C++ standards, without any impact on performance or code size.

### References



---

### CMSIS RTOS API: Avoid the heavy use of macros (to define objects and to refer to them) (#36)

All `*Def` structures passed to the `*Create()` functions are created using macros, and all these macros, generally used in places where variables are defined, look like function calls, but are not functions, they are structures, making the definitions quite confusing, both for humans and for code analysers.

For example:

```
#include "cmsis_os.h"
 
void Thread_1 (void const *arg);                   // function prototype for Thread_1
osThreadDef (Thread_1, osPriorityNormal, 1, 0);    // define Thread_1
 
int main (void) {
  osThreadId id;
  
  id = osThreadCreate (osThread (Thread_1), NULL); // create the thread
  if (id == NULL) {                                // handle thread creation
    // Failed to create a thread
  }
  
  osThreadTerminate (id);                          // stop the thread
}
```

Suggestion:

Give up using all these macros and find alternate, but more standard, solutions.

For example, [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) uses the POSIX approach, with optional attributes. It provides both:

- a default, simple, creation method, with minimum arguments and a null pointer to the attributes
- a complete, rich in options, expandable, creation methood, using the attributes.

In CMSIS++, even in the C API, all objects are created with a user supplied storage (one of the structures defined in the `cmsis-plus/rtos/os-c-decls.h`).

For example:

```
...
os_thread_t th1;
os_thread_create(&th1, NULL, func, args);
...
os_thread_destroy(&th1);
...
```

This pattern is consistent acress all objects, and matches the use of  the C++ `this` pointer as the first argument to all object methods.


### CMSIS RTOS API: Do not make the use of a dynamic allocator mandatory (for stack, queues, etc) (#37)

Although the macros defining objects can be configured to statically allocate storage for objects, some of them internally still allocate storage on the heap, for example for the thread stack.

In CMSIS RTOS running on RTX, I could not find a way to avoid these allocations.

Suggestion:

* add attributes and allow to pass pointers to user defined
storage.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) all objects can be created with one or more user supplied storages, which can be allocated either statically, or on the stack, or on the heap, according to user needs. 



### CMSIS RTOS API: Add support for critical regions (interrupts & scheduler) (#38)

To my big surprise, the current CMSIS RTOS API specs do not inlude any means to define critical regions.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), depending on the purpose of the protected region, the user can define regions
where some interrupts are not allowed, or regions where the scheduler context switches are not allowed.

Suggestion:

* define interrupts critical regions (with interupts masked/restored)
* define scheduler criticals regions (with scheduler lock/unlock)

```

  scheduler_state_t
  osSchedulerLock (void);

  void
  osSchedulerUnlock (scheduler_state_t);

  // ...

  interrupts_state_t
  osCriticalEnter (void);

  void
  osCriticalExit (interrupts_state_t);

```

Please note that the only totally safe strategy for implementing nested critical sections is to save the status on enter and restore it on exit. Attempts to use nesting counters are not only more complicated, but do not cover all possible sitations and in some circumstances are even error prone.


### CMSIS RTOS API: Avoid mixing time durations (in milliseconds) with timer counts in ticks (#39)

Although the usual system timer is set for 1000 ticks per second, which corresponds to 1 ms per tick, this is not mandatory, and some applications may very well use different timer settings.

Since all system timeouts are actually implemented as timer ticks, it is more accurate to define durations in number of ticks, not milliseconds, for all timeouts.

This is also both a POSIX requirement, and a ISO C/C++ specs reqirement, to use clock ticks and round timings to the next clock interval.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) all timeouts have the type `clock::duration_t` and are expressed in clock specific units, either ticks or seconds. For those who want to use timeouts in milliseconds, CMSIS++ provides an inline function to convert to timer ticks).

Suggestion:

* change all prototypes to use ticks instead of milliseconds


### CMSIS RTOS API: Add a separate RTC system clock (#40)

The SysTick derived clock is fine for short timeouts, but cannot be used for applications that go into deep sleep (when the SysTick is no longer active) and need to wakeup at specific intervals.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) there are two system clocks; one is the Systick\_clock, a steady clock that counts ticks, and the Realtime\_clock, an adjustable clock that counts seconds, and can be initially adjusted to reflect the absolute time from the epoch (Jan 1st, 1970) and can also be periodically adjusted using a reference clock.

With multiple clocks, all the objects that define timeouts can be configured to use either the default SysTick clock or the RTC clock, so
the timeout duration depends on the selected clock.

If a hardware RTC is not available, the CMSIS++ RTOS can be configured to derive the seconds from the SysTick clock.

The presence of a RTC clock is consistent with POSIX and ISO requirement to have steady and adjustable clocks.


### CMSIS RTOS API: Add os_main() to make explicit the use of a main thread (#41)

One of the inovations I appreciated at CMSIS RTOS API was the possibility to make the main() function run as a thread.

However, while working on [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), I identified two problems with this approach:

- the solutions to implement this feature are not very portable, making almost impossible to run CMSIS test applications in synthetic environments, like POSIX user processes;
- being an optional feature that some CMSIS ports may choose not to implement, the application code is actually more complicated than without this feature, since it must check if the feature is available or not, and possibly start the scheduler, without being able to use other sysm calls in main().

The solution that I used in CMSIS++ was to do not request for this feature  to be implemented at system level, but move it to user space, as an explicit `os_main()` function, always available as a weak function, that runs in the context of a thread started from the standard `main()`.

With this approach, the user is free to choose how it prefers to start the application. If the user wants to have a main that runs in the context of a thread, uses `os_main()`; if not, the standard main() is available, and here the user can handle the scheduler initialisations explicitly, in any custom way.


### CMSIS RTOS API: Add support for a synchronised public memory allocator (#42)

Although internally the CMSIS RTOS implementations (like RTX, FreeRTOS) require an allocator for some objects  (like thread stacks), the application might also need to allocate memory for specific purposes, and it is not specified how the public allocator should behave, especially how it should handle inter-thread synchronisation.

On top of the standard malloc()/free(), [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) defines several functions  in the `estd::` namespace, that provide synchronised access to the memory allocator. 

Suggestion:

* add support for synchronised malloc()/free(); syncronisation can be done either by mutex or scheduler critical sections (assuming no allocations will be performed on interrupts).

### CMSIS RTOS API: Avoid returning aggregates (like osEvent) (#43)

Although allowed by the C language, and supported by most compilers, returning structures directly is not a good ideea, since it will trigger lots of warnings in build environments where all warnings are enabled.

Suggestion:

* redesign the API to not return structures, not even structures with bitfields.



### CMSIS RTOS API: Extend the range for osKernelSysTick() (#44)

The 32-bits value of the returned counter allows only small time ranges.

For example, for a 100 MHz CPU, at 1000 ticks/sec, the SysTick counter divisor is 100.000, or 10^5, allowing only 4*10^(9-5), or 40000 ticks, which represent only 40 seconds between rollovers.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), the SysTick clock can return separate values in a user provided  structure, allowing any kind of computations related to accurate timings.

Suggestion:

* widen the value to 64-bits.
* return separate ticks & cycles in an optional structure

#### CMSIS RTOS API: Use explicit separate calls for different waiting functions (#45)

In POSIX there are three different functions, for example to lock a mutex:

```
  lock();
  try_lock();
  timed_lock();
```

In CMSIS RTOS API all are multiplexed via a single call, the  differentiator being the value of the timeout:

- with 0xFFFFFFFF to mean _wait forever_ (for `lock()`)
- with 0, to mean _try, do not block_ (for `try_lock()`)
- with any other value to mean _wait with timeout_ (for `timed_lock()`)

Although functionally equivalent, I think that the POSIX design increases readability  and this is the pattern used in [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) for all waiting functions.

Aside from increased readability, another advantage is that some functions like `try_alloc()` can be used in ISRs, while blocking functions like `alloc()` or `timed_alloc()` cannot, and should return an error code.

With the current CMSIS RTOS API, the distinction between function allowed in ISR is not as obvious, for example `osMessageGet()` is specified to be allowed in ISR, but actually it is allowed only with a timeout value of 0.

Suggestion:

* add separate functions like `lock()`, `try_lock()`, `timed_lock()`.

Candidate functions:

* `osSignalWait()`
* `osSemaphoreWait()`
* `osMessagePut()`
* `osMessageGet()`
* `osMailAlloc()`
* `osMailCAlloc()`
* `osMailGet()`
* `osMutexWait()`


### CMSIS RTOS API: Make the scheme to assign names to objects more consecvent (#46)

The macros used to define object names are not consecvent, for example the  thread name is derived from the thread function name, but the timer name should be explicitly defined, different from the timer function.

```
void Thread_1 (void const *arg);                          
osThreadDef (Thread_1, osPriorityNormal, 1, 0); 

void Timer1_Callback  (void const *arg);                  
osTimerDef (Timer1, Timer1_Callback);   

```

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), all objects attributes include the `.name` field, making this mechanism  consistent across all objects.

Suggestion:

* use separate explicit names for all objects.

### CMSIS RTOS API: Add missing destructor functions to all objects (#47)

Some objects can be deleted, like threads, timers and mutexes, but others  cannot (like pools, message queues, mailboxes).

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) all C++ objects have proper destructors, and the C API wraps them with `os_*_destroy()` calls.

Suggestion:

* extend the API to be sure all created objectc have destructors (for example add osPoolDelete(), osMessageDelete(), osMailDelete()).

Note: apparently addressed in CMSIS 5, but must be checked.


### CMSIS RTOS API: Use POSIX error codes (#65)

The CMSIS RTOS API defines a set of error codes, but the semantic is proprietary (and, to my opinion, not very consistent).

Based on POSIX, [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) uses the standard POSIX error codes from the <errno.h> header file, the only specific definition being `result::ok` to indicate that no error occurred.

Suggestion:

* use the standard POSIX error codes instead of reinventing new codes.


### CMSIS RTOS API: Add typedefs for all different types used in prototypes (#66)

It is a good programming practice to define separate types for each different function parameter, instead of using generic `uint32_t`; done properly this usually increases readability.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) all types have separate definitions (for example: `clock::duration_t`, `scheduler::state_t`, `flags::mask_t`, `thread::priority_t`, and so on).

Suggestion:

* add typedefs for all different types used in prototypes


#### CMSIS RTOS API: For all objects, add reset functions to return the object to initial status (#67)

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) objects can be returned to the initial status with `reset()`.

Suggestion:

* add reset() to the queue object
* add reset() to the memory pool object
* add reset() to the mail object
* add reset() to the semaphore object
* add reset() to the mutex object

### Threads

#### CMSIS Documentation: Explain that thread functions can return (#48)

The specs currently do not define what happens if a thread exits, although  the validator makes extensive use of thread termination, and thread reuse.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), a thread can terminate either by returning from the thread  function, or by explicitly calling `Thread::exit()`; alternatively, another thread can use `Thread::kill()` to terminate another thread.

Suggestion:

* add explicit mentions of thread temination, and of thread reuse


#### CMSIS RTOS API: Extend the range of priority levels (#49)

In CMSIS RTOS API there are only 7 thread priorityes, including idle, which might not be enough.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) the priorities are stored as a byte, and the range (up to 255) can be configured by a user definition.

Suggestion:

* extend the range to 256 levels, and represent it as unsigned (0-255).

Note: apparently addressed in CMSIS 5, but the range is smaller, and still uses enumerated values.


#### CMSIS RTOS API: Add a mechanism to wait for a thread to terminate (#50)

The current specs do not define what happens if a thread exits, and there is no mechanism to tell is a thread has terminated or not.

According to POSIX, in [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) threads have a `join()` method, that can be used by other threads (usually the parent thread) to wait for thread termination.

Suggestion:

* add `join()`, to allow one thread to wait for a thread to terminate


#### CMSIS RTOS API: Add a mechanism to enumerate all registered threads (#51)

For writing monitor commands that list all available threads, it is required to enumerate existing threads.

Suggestion:

* add a standard iterator to allow access to all scheduler registered threads.


### Mutexes

#### CMSIS Documentation: Explain the mutex behaviour (recursive vs normal) (#52)

The current [docs](http://www.keil.com/pack/doc/CMSIS/RTOS/html/group___c_m_s_i_s___r_t_o_s___mutex_mgmt.html) do not explicitly specify if the mutex is normal or recursive. However, RTX implements recursive mutexes (only), and the validator expect the mutex to  be recursive.

Suggestion:

* update the documentation to explain the expected mutex behaviour.


#### CMSIS RTOS API: Add normal (non-recursive) mutex (#53)

POSIX defines several mutex types, like normal vs recursive.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), the attributes used to create mutexes include all  POSIX functionality, included type, robustness, priority ceiling, max count.

Suggestion:

* add an explicit method to create normal and recursive mutexes.

#### CMSIS RTOS API: For mutex, add a method to get the owner thread (#68)

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), if a mutex is locked, it is possible to know which thread locked the mutex.

Suggestion:

* add an `owner()` function to return a pointer to the thread that owns the mutex
  
### Semaphores

#### CMSIS Documentation: Clarify the specs for binary vs counting semaphores (#54)

The specs mention that creating a semaphore with a count=1 is equivalent to creating a binary semaphore, but expected functionality is not clearly presented.

Suggestion:

* add explicit binary vs counting semaphore definitions, with separate examples.


#### CMSIS RTOS API: Allow to explicitly define the semaphore max count (#55)

On one hand, the current specs use the count value both as initial count and as max count, and on the other hand they allow for this count to be 0 (to distinguish between producer and consumer threads).

This is confusing, and leads to unusual code, for example when a max count must be defined, but the semaphore must start with no tokens, a loop of `osSemaphoreWait()` calls must be performed, to return the count to 0.

An explicit max count also has the advantage of making the definitions of binary semaphores obvious (max count is 1).

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), the attributes used for semaphore creation include both an initial and a max count. The default semaphore is created with
the initial count as 0, and MAX_INT for the max count.

Suggestion:

* add an explicit max_count.


#### CMSIS RTOS API: Make osSemaphoreWait() return errors, not counts (#56)

`osSemaphoreWait()` returns the semaphore count, no errors like all other calls. At least one CMSIS OS port (the ST one) got this wrong, and returned errors.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), the `wait()`/`try_wait()`/`timed_wait()` all return error codes, and the semaphore count is obtained by the separate `Semaphore::value()` call.

Suggestion

* change osSemaphoreWait() to return status codes
* add a separate function to return the semaphore count


### Memory pools

#### CMSIS RTOS API: Add a method to wait for a memory pool block to become available (#57)

The function `osPoolAlloc()` just returns NULL if there are no more available blocks, but does not provide a way to wait for a block.

This design seems fit for use with interrupt routines, where waiting is not an option, but makes things quite difficult when memory blocks are required by a thread.

Also, the design of `osMemoryPool` is not consistent with the design of  `osMailQueue`, which provides blocking calls.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), the Memory\_pool object is consistent with all other objects, it can be used either in ISR contexts, with `try_alloc()` or in thread contexts, with the blocking `alloc()` and `timed_alloc()`.


Suggestion:

* define all three blocking and non-blocking calls

```
  alloc();
  try_alloc();
  timed_alloc();
```

#### CMSIS RTOS API: For memory pools, add more accessors to get pool status (#69)

In CMSIS RTOS API, once created, a memory pool has no functions to tell if the pool is empty or full, to get the block size, the pool capacity, and so on.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) all memory pool characteristics are available via accessor functions.

```
std::size_t
capacity (void) const;

std::size_t
count (void) const;

std::size_t
block_size (void) const;

bool
empty (void) const;

bool
full (void) const;
```

Suggestion:

* add accessor functions for memory pool status



### Message queues


#### CMSIS Documentation: Fix the data type used in osMessageQDef() example (#58)

The current documentation 
[page](http://www.keil.com/pack/doc/CMSIS/RTOS/html/group___c_m_s_i_s___r_t_o_s___message.html)  defines the `type` parameter as _data type of a single message element_, which for message queues should be either int or pointer.

However, the example in `osMessageCreate()` passes a structure, not a pointer to the structure.

```
typedef struct {                                 // Message object structure
  float    voltage;                              // AD result of measured voltage
  float    current;                              // AD result of measured current
  int      counter;                              // A counter value
} T_MEAS;
 
osMessageQDef(MsgBox, 16, T_MEAS);               // Define message queue
```

In the RTX implementation of CMSIS RTOS this works because the data type is ignored by the `osMessageQDef()` macro, being replaced by an `uint32_t` (which, BTW, is not a portable way to store pointers):

```
#define osMessageQDef(name, queue_sz, type)   \
uint32_t os_messageQ_q_##name[4+(queue_sz)] = { 0 }; \
const osMessageQDef_t os_messageQ_def_##name = \
{ (queue_sz), (os_messageQ_q_##name) }
```


#### CMSIS Documentation: Fix misplaced thread id parameter for message queue (#59)

In the [Message Queue](http://www.keil.com/pack/doc/CMSIS/RTOS/html/group___c_m_s_i_s___r_t_o_s___message.html) page the explanation note about _thread_ is listed under `osMessageQDef()` instead of `osMessageCreate()` (which generally ignores it, anyway).


#### CMSIS RTOS API: Fix non-portable message type in osMessagePut() (#60)

The parameter used to pass messages is `uint32_t info` even when a pointer is required.

In particular, for 32-bits cores, this can be fixed with casts, but  for 64-bits platforms this is no longer possible.

As a curious thing, internally RTX uses `void*` as message type, but, for unknown reasons, the CMSIS definition ended up with an integer instead of a pointer.

Suggestion:

* update the queue specs to explicitly store pointers, not integers


#### CMSIS RTOS API: Deprecate or remove the unused thread_id parameter in osMessageQCreate()/osMailQCreate() prototypes (#61)

The parameter is defined as _Note: The parameter thread registers the receiving thread for a message and is needed for the general osWait function to deliver the message._

However both the RTX and the FreeRTOS implementations ignore this parameter.




#### CMSIS RTOS API: Fix osMessagePut()/osMailPut() inconsecvent error when called from ISR (#62)

Most other calls return `osErrorISR` when called from ISR.

`osMessagePut()` (and its sister `osMailPut()`), when called from ISR with a timeout different from 0, returns `osErrorParameter`, as if the problem would be the parameter, not the context where it is called from.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) this problem is simply avoided by having distinct calls, with all blocking calls banned from ISR, and all `try_xxx()` calls allowed from ISRs.

#### CMSIS RTOS API: For message queues, make the message size user configurable (#70)

The current queue specifications limits the message size to an integer (and RTX internally uses a void*).

Based on POSIX, [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) does not limit the message size; instead it is configured at queue creation.

Suggestion:

* extend the queue creation API and add the message size



#### CMSIS RTOS API: For message queues, add more accessors to get queue status (#71)

In CMSIS RTOS API, once created, a queue has no functions to tell if the queue is empty or full, to get the message size, the queue capacity, and so on.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) all queue characteristics are available via accessor functions.

```
std::size_t
capacity (void) const;

std::size_t
length (void) const;

std::size_t
msg_size (void) const;

bool
empty (void) const;

bool
full (void) const;
```

Suggestion:

* add accessor functions for queue status

#### CMSIS RTOS API: For message queues, add message priorities (#72)

The current queue specifications do not mention anything about message priorities, apparently all messages have equal priority and the queue policy seems to be FIFO.

Based on POSIX, in [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) message queues have priorities, and messages are retrieved in priority order.

Suggestion:

* extend the API and add priorities to the functions storing/retrieving messages from the queue



### Mail queues

#### CMSIS RTOS API: Mail queues, as separate objects, are redundant (#63)

A mail queue is a combination of a memory pool and a message  queue, with very similar usage, which makes it redundant.

Actually in [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), with carefully designed C++ objects, it was perfectly possible to implement the `osMailQ` by composing it from a pool and a queue.

```
#define osMailQStaticDef(name, items, type) \
struct { \
    osMailQ data; \
    struct { \
      type pool[items]; \
    } pool_storage; \
    struct { \
      void* queue[items]; \
      os_mqueue_index_t links[2 * items]; \
      os_mqueue_prio_t prios[items]; \
    } queue_storage; \
} os_mailQ_##name; \
const osMailQDef_t os_mailQ_def_##name = { \
    #name, \
    (items), \
    sizeof (type), \
    sizeof (void*), \
    &os_mailQ_##name.pool_storage, \
    sizeof(os_mailQ_##name.pool_storage), \
    &os_mailQ_##name.queue_storage, \
    sizeof(os_mailQ_##name.queue_storage), \
    &os_mailQ_##name.data \
}
```

### CMSIS 5

#### CMSIS RTOS API v2: Move deprecated definitions to a separate file (#75)

Browsing through the proposal for the new `cmsis_os.h`, I noticed that perhaps more than half of it is now deprecated and requires the presence of the `osCMSIS_API_V1` macro.

Advanced editors like Eclipse can show the unused lines as grey, but it is still difficult to read such a file.

Suggestion:

* move all deprecated definitions to a separate file, and explicitly mark them as 'Not recommended for new designs'



#### CMSIS RTOS API v2: Avoid mandatory dynamic allocation for all objects inadequate due to fragmentation (#76)

The first major change I noticed in RTOS API v2 reads:

```
OS object's resources dynamically allocated rather than statically:
- added: osXxxxNew functions which replace osXxxxCreate
- removed: osXxxxCreate functions, osXxxxDef_t structures
```

Implementing this requires either a set of statically configured pools for each object type (separate pools for threads, mutexes, semaphores, pools, queues, mailboxes, flags, etc), which is highly discouraged, or the use of a true dynamic allocator, which is inacceptable for many applications due to fragmentation.

[CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) gives the user full control of the location where the objects are allocated, similar to C++, in other words objects can be statically allocated in the BSS, on the thread stack or on the heap (with a dynamic allocator like malloc()).

Suggestion:

* add a first parameter to the object creation function, with a pointer to a structure where the object will be emplaced; if this pointer is null, the object will be allocated dynamically using an implementation specific allocator.

#### CMSIS RTOS API v2: Avoid adding further macros for the attributes (#77)

I noticed that, probably inspired by CMSIS++, you added attributes to control the creation of new objects. This is actually the POSIX approach, and I think it is ok.

However you also added lots of macros to initialise these attributes, and this is not really POSIX, and not really a good idea.

The major POSIX argument for using attributes, is that they can be easily extended in future versions, without breaking compatibility with existing versions.

For this to work, POSIX has functions that initialise the attributes to convenient defaults, then each attribute is set/get, in a single line, with a specialised setter/getter. In POSIX, adding a new parameter requires only adding a new setter/getter, which does not break compatibility.

In CMSIS RTOS API v2 you use macros with lots of arguments, which may be compact, but if in a future version you'll need to add a new argument, you'll have to either extend the macro, or set the attribute manually in the structure, which is inconsistent.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) all attribute objects are initialised with a similar looking constructor, with a single argument (the object name). All other attributes are consistently set by directly manipulating the structure members.

For example, in the compatibility wrapper, the implementation of `osThreadCreate()` includes:

```
  thread::Attributes attr
    { thread_def->name };
  attr.th_priority = thread_def->tpriority;
  attr.th_stack_size_bytes = thread_def->stacksize;
  ...
  new (th) Thread (attr, (thread::func_t) thread_def->pthread, args);
```

Suggestion:

* use a consistent, similar loooking, macro to initialise all objects with default values and explicitly set structure members for each attribute.

#### CMSIS RTOS API v2: When using attributes, avoid unnecessary creation parameters (#78)

One of the POSIX goals when designing the attributes mechanism was to remove all non-mandatory parameters in object creation functions.

For example, when creating a thread, the user function and the arguments are mandatory, but the priority, stack size and other such details can have defaults and were moved to attributes.

[CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) strictly follows this principle.

The RTOS API v2 design does not, and inconsistently messes attributes with non-mandatory parameters, for example:

- the semaphore creation `osSemaphoreNew()` requires `max_count` and `initial_count`, although there are reasonable defaults for both of them (max_int and 0, for example)
- the `osMemoryPoolNew()`, `osMessageQueueNew()` and `osMailQueueNew()` functions require `memory`, although it shouldn't be mandatory, since the storage can be dynamically allocated (also see the more specific #???)

Suggestion:

* move non-mandatory parameters to attributes


#### CMSIS RTOS API v2: Inconsistent memory allocation for object and object storage (#79)

For memory pools, message queues and mail queues, the proposed APIs assumes mandatory dynamic allocation for the objects themselves, but also expect user allocated buffers for the pools and/or queues.

This is inconsistent; if dynamic application is appropriate for the application, it should also be possible to automatically allocate the pools/queues storage, with minimum user intervention.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), all objects requiring storage can be configured with user provided buffers, but these buffers are passed as pointer + size via the creation attributes.

Suggestion:

* move the non-mandatory `memory` parameter to attributes; if not specified, dynamically allocate the required storage. (this is a more specific version of #78). For validation purposes, also add a `storage_size` attribute, to be sure the desired storage fits the user provided buffer.


#### CMSIS RTOS API v2: Fix inconsistent type naming convention (#80)

This is a recurring problem, that seems to affect all CMSIS components, not only RTOS, and, if I remember right, I already reported, but apparently without effect.

In the current `cmsis_os.h` I can read the following types:

* `osThreadState`
* `os_timer_type`
* `osThreadAttr_t`

In other words you freely mess camel case names with underscores, with or without the standard `_t` suffix.

Even worse, types are not consisten even inside a single definition (the struct name uses one naming convention and the typedef another naming convention):

```
typedef struct os_mutex_attr {
  const char                 *name;    
  uint32_t               attr_bits;    
  uint32_t                reserved;    
} osMutexAttr_t;

```

In my opinion your general naming convention, not only the type naming convention, is problematic, but the issue here is not which naming convention you choose (this may turn into a religious war), but once you choose it, stick to it consistently.

[CMSIS++](http://micro-os-plus.github.io/cmsis-plus/) uses the lower case naming convention, also adopted by POSIX and by the ISO C/C++ standards, with types suffixed by `_t`. User class names may have the initial character in uppercase.

Suggestion

* make the type naming convention uniform, preferably using the POSIX convention, with lower case and the `_t` suffix.


#### CMSIS RTOS API v2: osEventFlagsWait()/osThreadFlagsWait() return type inconsistent with other blocking calls (#81)

Most blocking calls return a status code, usually ok or timeout.

It is not clear how you can differentiate the ok from timeout, in case of the flags functions, but it is inconsistent.

In [CMSIS++](http://micro-os-plus.github.io/cmsis-plus/), all blocking functions return a consistent status code, and the output flags are passed via a pointer parameter.

http://micro-os-plus.github.io/reference/cmsis-plus/classos_1_1rtos_1_1_event__flags.html

Suggestion:

- change the return type to a status code and add a pointer parameter for the output flags


---


### Documentation



* the notice _MUST REMAIN UNCHANGED_ applied to enums should be corrected to state that only the names must remain unchanged, there is no special reason for the actual values of the error codes, the priorities,  and other enums to be restricted.

* the `osThreadDef()` does not explain that the parameter `instances` means that the macro reserves space for the given number of threads, and a single definition can be used to instantiate multiple threads att he same time.

* the `osThreadDef()/osThreadCreate()` do not explain if it is possible to reuse a thread definition, and in what conditions; a terminated thread definition may be reused; if is given the definition of an active thread, `osThreadCreate()` should return NULL; if multiple instances are defined, they can be dinamically reused, as soon as one thread is terminated, its definition can be reused.
 
* the `osSemaphoreWait()` is documented to return _the number of available tokens (the semaphore count value)_. If non zero, this number means the count value before the call, or the number of available tokens before taking one by the current call.

In other words, for a semaphore initialised with 1 token, the first wait() acuires the token and returns 1; all subsequent non blocking waits return 0.

POSIX defines explicit `sem_getvalue()`, which returns the instantaneous value of the counter; `sem_wait()` does not return the current value, like CMSIS.

* it is not documented what `osMessagePut()` should return when called from ISR with timeout not zero; the validator expects osErrorParameter, but osErrorISR would be more appropriate.

## RTX

### Does not use CMSIS core headers

As a significant prove that CMSIS CORE definitions are not generic enough, the Keil RTX does not use CMSIS CORE, but defines all system peripherals internally.

### Does not use standard ISO C type definitions (like uint32_t)

Although there are more than 15 years since the ISO C standard defined how integer definitions should look like, the Keil RTX still prefers to ignore them and uses custom definitions like U8, U16, etc.

## CMSIS CORE

The split of core files by families (core\_cm0, core\_cm3...) is not only redundant, since many definitions are the same, but olso makes use  of the common definitions (like SysTick, NVIC) imposible for generic applications.

## CMSIS Drivers 

* uses non-reentrant callbacks
* should not return aggregates (version, capabilities, status)
* Initialize() enable clocks, which means it powers up the device
* callbacks registered together with hardware inits
* initialize/unitialize must be linked to power up/down

Driver implementation

* poor code reusability: when writing multiple similar implementations it is more difficult to share code between them
* drivers are defined as separate pointers to RO struct of functions, instead of a common set of functions and different pointers to data

### USART

* configuration calls mixed with operational commands

### USBD

* inconsecvent status name (ARM\_USBD\_STATE, vs ARM\_USART\_STATUS)
