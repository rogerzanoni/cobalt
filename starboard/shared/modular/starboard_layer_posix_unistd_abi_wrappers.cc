// Copyright 2024 The Cobalt Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "starboard/shared/modular/starboard_layer_posix_unistd_abi_wrappers.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "starboard/shared/modular/starboard_layer_posix_errno_abi_wrappers.h"
#include "starboard/shared/modular/starboard_layer_posix_fcntl_abi_wrappers.h"
#include "starboard/shared/modular/starboard_layer_posix_stat_abi_wrappers.h"
#include "starboard/shared/modular/starboard_layer_posix_unistd_abi_wrappers.h"

namespace {
// Returned by musl_conf_to_platform_conf() for a name that is
// a valid in musl but not supported by the platform.
constexpr int kMuslConfUnsupported = -2;

int musl_conf_to_platform_conf(int name) {
  switch (name) {
    case MUSL_PC_LINK_MAX:
#if defined(_PC_LINK_MAX)
      return _PC_LINK_MAX;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_LINK_MAX)
    case MUSL_PC_MAX_CANON:
#if defined(_PC_MAX_CANON)
      return _PC_MAX_CANON;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_MAX_CANON)
    case MUSL_PC_MAX_INPUT:
#if defined(_PC_MAX_INPUT)
      return _PC_MAX_INPUT;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_MAX_INPUT)
    case MUSL_PC_NAME_MAX:
#if defined(_PC_NAME_MAX)
      return _PC_NAME_MAX;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_NAME_MAX)
    case MUSL_PC_PATH_MAX:
#if defined(_PC_PATH_MAX)
      return _PC_PATH_MAX;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_PATH_MAX)
    case MUSL_PC_PIPE_BUF:
#if defined(_PC_PIPE_BUF)
      return _PC_PIPE_BUF;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_PIPE_BUF)
    case MUSL_PC_CHOWN_RESTRICTED:
#if defined(_PC_CHOWN_RESTRICTED)
      return _PC_CHOWN_RESTRICTED;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_CHOWN_RESTRICTED)
    case MUSL_PC_NO_TRUNC:
#if defined(_PC_NO_TRUNC)
      return _PC_NO_TRUNC;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_NO_TRUNC)
    case MUSL_PC_VDISABLE:
#if defined(_PC_VDISABLE)
      return _PC_VDISABLE;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_VDISABLE)
    case MUSL_PC_SYNC_IO:
#if defined(_PC_SYNC_IO)
      return _PC_SYNC_IO;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_SYNC_IO)
    case MUSL_PC_ASYNC_IO:
#if defined(_PC_ASYNC_IO)
      return _PC_ASYNC_IO;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_ASYNC_IO)
    case MUSL_PC_PRIO_IO:
#if defined(_PC_PRIO_IO)
      return _PC_PRIO_IO;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_PRIO_IO)
    case MUSL_PC_SOCK_MAXBUF:
#if defined(_PC_SOCK_MAXBUF)
      return _PC_SOCK_MAXBUF;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_SOCK_MAXBUF)
    case MUSL_PC_FILESIZEBITS:
#if defined(_PC_FILESIZEBITS)
      return _PC_FILESIZEBITS;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_FILESIZEBITS)
    case MUSL_PC_REC_INCR_XFER_SIZE:
#if defined(_PC_REC_INCR_XFER_SIZE)
      return _PC_REC_INCR_XFER_SIZE;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_REC_INCR_XFER_SIZE)
    case MUSL_PC_REC_MAX_XFER_SIZE:
#if defined(_PC_REC_MAX_XFER_SIZE)
      return _PC_REC_MAX_XFER_SIZE;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_REC_MAX_XFER_SIZE)
    case MUSL_PC_REC_MIN_XFER_SIZE:
#if defined(_PC_REC_MIN_XFER_SIZE)
      return _PC_REC_MIN_XFER_SIZE;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_REC_MIN_XFER_SIZE)
    case MUSL_PC_REC_XFER_ALIGN:
#if defined(_PC_REC_XFER_ALIGN)
      return _PC_REC_XFER_ALIGN;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_REC_XFER_ALIGN)
    case MUSL_PC_ALLOC_SIZE_MIN:
#if defined(_PC_ALLOC_SIZE_MIN)
      return _PC_ALLOC_SIZE_MIN;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_ALLOC_SIZE_MIN)
    case MUSL_PC_SYMLINK_MAX:
#if defined(_PC_SYMLINK_MAX)
      return _PC_SYMLINK_MAX;
#else
      return kMuslConfUnsupported;
#endif  // defined(_PC_SYMLINK_MAX)
    case MUSL_PC_2_SYMLINKS:
#if defined(_PC_2_SYMLINKS)
      return _PC_2_SYMLINKS;
#else
      return kMuslConfUnsupported;
#endif        // defined(_PC_2_SYMLINKS)
    default:  // Explicitly handle unsupported names
      errno = EINVAL;
      return -1;
  }
}

int access_helper(int musl_amode) {
  int platform_amode = 0;
  if (musl_amode == MUSL_F_OK) {
    return F_OK;
  }
  if (musl_amode & MUSL_R_OK) {
    platform_amode |= R_OK;
  }
  if (musl_amode & MUSL_W_OK) {
    platform_amode |= W_OK;
  }
  if (musl_amode & MUSL_X_OK) {
    platform_amode |= X_OK;
  }
  return platform_amode;
}

int musl_unlink_flag_to_platform_flag(int musl_flag) {
  switch (musl_flag) {
    case 0:
      return 0;
    case MUSL_AT_REMOVEDIR:
      return AT_REMOVEDIR;
    default:
      errno = EINVAL;
      return -1;
  }
}
}  // namespace

int __abi_wrap_ftruncate(int fildes, musl_off_t length) {
  return ftruncate(fildes, static_cast<off_t>(length));
}

musl_off_t __abi_wrap_lseek(int fildes, musl_off_t offset, int whence) {
  return static_cast<off_t>(lseek(fildes, static_cast<off_t>(offset), whence));
}

long __abi_wrap_sysconf(int name) {
  switch (name) {
    case MUSL_SC_ARG_MAX:
#if defined(_SC_ARG_MAX)
      return sysconf(_SC_ARG_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_ARG_MAX)
    case MUSL_SC_CHILD_MAX:
#if defined(_SC_CHILD_MAX)
      return sysconf(_SC_CHILD_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_CHILD_MAX)
    case MUSL_SC_CLK_TCK:
      return sysconf(_SC_CLK_TCK);
    case MUSL_SC_NGROUPS_MAX:
#if defined(_SC_NGROUPS_MAX)
      return sysconf(_SC_NGROUPS_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_NGROUPS_MAX)
    case MUSL_SC_OPEN_MAX:
#if defined(_SC_OPEN_MAX)
      return sysconf(_SC_OPEN_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_OPEN_MAX)
    case MUSL_SC_STREAM_MAX:
#if defined(_SC_STREAM_MAX)
      return sysconf(_SC_STREAM_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_STREAM_MAX)
    case MUSL_SC_TZNAME_MAX:
#if defined(_SC_TZNAME_MAX)
      return sysconf(_SC_TZNAME_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TZNAME_MAX)
    case MUSL_SC_JOB_CONTROL:
#if defined(_SC_JOB_CONTROL)
      return sysconf(_SC_JOB_CONTROL);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_JOB_CONTROL)
    case MUSL_SC_SAVED_IDS:
#if defined(_SC_SAVED_IDS)
      return sysconf(_SC_SAVED_IDS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SAVED_IDS)
    case MUSL_SC_REALTIME_SIGNALS:
#if defined(_SC_REALTIME_SIGNALS)
      return sysconf(_SC_REALTIME_SIGNALS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_REALTIME_SIGNALS)
    case MUSL_SC_PRIORITY_SCHEDULING:
#if defined(_SC_PRIORITY_SCHEDULING)
      return sysconf(_SC_PRIORITY_SCHEDULING);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_PRIORITY_SCHEDULING)
    case MUSL_SC_TIMERS:
#if defined(_SC_TIMERS)
      return sysconf(_SC_TIMERS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TIMERS)
    case MUSL_SC_ASYNCHRONOUS_IO:
#if defined(_SC_ASYNCHRONOUS_IO)
      return sysconf(_SC_ASYNCHRONOUS_IO);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_ASYNCHRONOUS_IO)
    case MUSL_SC_PRIORITIZED_IO:
#if defined(_SC_PRIORITIZED_IO)
      return sysconf(_SC_PRIORITIZED_IO);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_PRIORITIZED_IO)
    case MUSL_SC_SYNCHRONIZED_IO:
#if defined(_SC_SYNCHRONIZED_IO)
      return sysconf(_SC_SYNCHRONIZED_IO);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SYNCHRONIZED_IO)
    case MUSL_SC_FSYNC:
#if defined(_SC_FSYNC)
      return sysconf(_SC_FSYNC);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_FSYNC)
    case MUSL_SC_MAPPED_FILES:
#if defined(_SC_MAPPED_FILES)
      return sysconf(_SC_MAPPED_FILES);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MAPPED_FILES)
    case MUSL_SC_MEMLOCK:
#if defined(_SC_MEMLOCK)
      return sysconf(_SC_MEMLOCK);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MEMLOCK)
    case MUSL_SC_MEMLOCK_RANGE:
#if defined(_SC_MEMLOCK_RANGE)
      return sysconf(_SC_MEMLOCK_RANGE);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MEMLOCK_RANGE)
    case MUSL_SC_MEMORY_PROTECTION:
#if defined(_SC_MEMORY_PROTECTION)
      return sysconf(_SC_MEMORY_PROTECTION);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MEMORY_PROTECTION)
    case MUSL_SC_MESSAGE_PASSING:
#if defined(_SC_MESSAGE_PASSING)
      return sysconf(_SC_MESSAGE_PASSING);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MESSAGE_PASSING)
    case MUSL_SC_SEMAPHORES:
#if defined(_SC_SEMAPHORES)
      return sysconf(_SC_SEMAPHORES);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SEMAPHORES)
    case MUSL_SC_SHARED_MEMORY_OBJECTS:
#if defined(_SC_SHARED_MEMORY_OBJECTS)
      return sysconf(_SC_SHARED_MEMORY_OBJECTS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SHARED_MEMORY_OBJECTS)
    case MUSL_SC_AIO_LISTIO_MAX:
#if defined(_SC_AIO_LISTIO_MAX)
      return sysconf(_SC_AIO_LISTIO_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_AIO_LISTIO_MAX)
    case MUSL_SC_AIO_MAX:
#if defined(_SC_AIO_MAX)
      return sysconf(_SC_AIO_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_AIO_MAX)
    case MUSL_SC_AIO_PRIO_DELTA_MAX:
#if defined(_SC_AIO_PRIO_DELTA_MAX)
      return sysconf(_SC_AIO_PRIO_DELTA_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_AIO_PRIO_DELTA_MAX)
    case MUSL_SC_DELAYTIMER_MAX:
#if defined(_SC_DELAYTIMER_MAX)
      return sysconf(_SC_DELAYTIMER_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_DELAYTIMER_MAX)
    case MUSL_SC_MQ_OPEN_MAX:
#if defined(_SC_MQ_OPEN_MAX)
      return sysconf(_SC_MQ_OPEN_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MQ_OPEN_MAX)
    case MUSL_SC_MQ_PRIO_MAX:
#if defined(_SC_MQ_PRIO_MAX)
      return sysconf(_SC_MQ_PRIO_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MQ_PRIO_MAX)
    case MUSL_SC_VERSION:
#if defined(_SC_VERSION)
      return sysconf(_SC_VERSION);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_VERSION)
    case MUSL_SC_PAGESIZE:
      return sysconf(_SC_PAGESIZE);
    case MUSL_SC_RTSIG_MAX:
#if defined(_SC_RTSIG_MAX)
      return sysconf(_SC_RTSIG_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_RTSIG_MAX)
    case MUSL_SC_SEM_NSEMS_MAX:
#if defined(_SC_SEM_NSEMS_MAX)
      return sysconf(_SC_SEM_NSEMS_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SEM_NSEMS_MAX)
    case MUSL_SC_SEM_VALUE_MAX:
#if defined(_SC_SEM_VALUE_MAX)
      return sysconf(_SC_SEM_VALUE_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SEM_VALUE_MAX)
    case MUSL_SC_SIGQUEUE_MAX:
#if defined(_SC_SIGQUEUE_MAX)
      return sysconf(_SC_SIGQUEUE_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SIGQUEUE_MAX)
    case MUSL_SC_TIMER_MAX:
#if defined(_SC_TIMER_MAX)
      return sysconf(_SC_TIMER_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TIMER_MAX)
    case MUSL_SC_BC_BASE_MAX:
#if defined(_SC_BC_BASE_MAX)
      return sysconf(_SC_BC_BASE_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_BC_BASE_MAX)
    case MUSL_SC_BC_DIM_MAX:
#if defined(_SC_BC_DIM_MAX)
      return sysconf(_SC_BC_DIM_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_BC_DIM_MAX)
    case MUSL_SC_BC_SCALE_MAX:
#if defined(_SC_BC_SCALE_MAX)
      return sysconf(_SC_BC_SCALE_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_BC_SCALE_MAX)
    case MUSL_SC_BC_STRING_MAX:
#if defined(_SC_BC_STRING_MAX)
      return sysconf(_SC_BC_STRING_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_BC_STRING_MAX)
    case MUSL_SC_COLL_WEIGHTS_MAX:
#if defined(_SC_COLL_WEIGHTS_MAX)
      return sysconf(_SC_COLL_WEIGHTS_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_COLL_WEIGHTS_MAX)
    case MUSL_SC_EXPR_NEST_MAX:
#if defined(_SC_EXPR_NEST_MAX)
      return sysconf(_SC_EXPR_NEST_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_EXPR_NEST_MAX)
    case MUSL_SC_LINE_MAX:
#if defined(_SC_LINE_MAX)
      return sysconf(_SC_LINE_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_LINE_MAX)
    case MUSL_SC_RE_DUP_MAX:
#if defined(_SC_RE_DUP_MAX)
      return sysconf(_SC_RE_DUP_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_RE_DUP_MAX)
    case MUSL_SC_2_VERSION:
#if defined(_SC_2_VERSION)
      return sysconf(_SC_2_VERSION);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_VERSION)
    case MUSL_SC_2_C_BIND:
#if defined(_SC_2_C_BIND)
      return sysconf(_SC_2_C_BIND);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_C_BIND)
    case MUSL_SC_2_C_DEV:
#if defined(_SC_2_C_DEV)
      return sysconf(_SC_2_C_DEV);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_C_DEV)
    case MUSL_SC_2_FORT_DEV:
#if defined(_SC_2_FORT_DEV)
      return sysconf(_SC_2_FORT_DEV);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_FORT_DEV)
    case MUSL_SC_2_FORT_RUN:
#if defined(_SC_2_FORT_RUN)
      return sysconf(_SC_2_FORT_RUN);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_FORT_RUN)
    case MUSL_SC_2_SW_DEV:
#if defined(_SC_2_SW_DEV)
      return sysconf(_SC_2_SW_DEV);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_SW_DEV)
    case MUSL_SC_2_LOCALEDEF:
#if defined(_SC_2_LOCALEDEF)
      return sysconf(_SC_2_LOCALEDEF);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_LOCALEDEF)
    case MUSL_SC_IOV_MAX:
#if defined(_SC_IOV_MAX)
      return sysconf(_SC_IOV_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_IOV_MAX)
    case MUSL_SC_THREADS:
#if defined(_SC_THREADS)
      return sysconf(_SC_THREADS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREADS)
    case MUSL_SC_THREAD_SAFE_FUNCTIONS:
#if defined(_SC_THREAD_SAFE_FUNCTIONS)
      return sysconf(_SC_THREAD_SAFE_FUNCTIONS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_SAFE_FUNCTIONS)
    case MUSL_SC_GETGR_R_SIZE_MAX:
#if defined(_SC_GETGR_R_SIZE_MAX)
      return sysconf(_SC_GETGR_R_SIZE_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_GETGR_R_SIZE_MAX)
    case MUSL_SC_GETPW_R_SIZE_MAX:
#if defined(_SC_GETPW_R_SIZE_MAX)
      return sysconf(_SC_GETPW_R_SIZE_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_GETPW_R_SIZE_MAX)
    case MUSL_SC_LOGIN_NAME_MAX:
#if defined(_SC_LOGIN_NAME_MAX)
      return sysconf(_SC_LOGIN_NAME_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_LOGIN_NAME_MAX)
    case MUSL_SC_TTY_NAME_MAX:
#if defined(_SC_TTY_NAME_MAX)
      return sysconf(_SC_TTY_NAME_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TTY_NAME_MAX)
    case MUSL_SC_THREAD_DESTRUCTOR_ITERATIONS:
#if defined(_SC_THREAD_DESTRUCTOR_ITERATIONS)
      return sysconf(_SC_THREAD_DESTRUCTOR_ITERATIONS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_DESTRUCTOR_ITERATIONS)
    case MUSL_SC_THREAD_KEYS_MAX:
#if defined(_SC_THREAD_KEYS_MAX)
      return sysconf(_SC_THREAD_KEYS_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_KEYS_MAX)
    case MUSL_SC_THREAD_STACK_MIN:
#if defined(_SC_THREAD_STACK_MIN)
      return sysconf(_SC_THREAD_STACK_MIN);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_STACK_MIN)
    case MUSL_SC_THREAD_THREADS_MAX:
#if defined(_SC_THREAD_THREADS_MAX)
      return sysconf(_SC_THREAD_THREADS_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_THREADS_MAX)
    case MUSL_SC_THREAD_ATTR_STACKADDR:
#if defined(_SC_THREAD_ATTR_STACKADDR)
      return sysconf(_SC_THREAD_ATTR_STACKADDR);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_ATTR_STACKADDR)
    case MUSL_SC_THREAD_ATTR_STACKSIZE:
#if defined(_SC_THREAD_ATTR_STACKSIZE)
      return sysconf(_SC_THREAD_ATTR_STACKSIZE);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_ATTR_STACKSIZE)
    case MUSL_SC_THREAD_PRIORITY_SCHEDULING:
#if defined(_SC_THREAD_PRIORITY_SCHEDULING)
      return sysconf(_SC_THREAD_PRIORITY_SCHEDULING);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_PRIORITY_SCHEDULING)
    case MUSL_SC_THREAD_PRIO_INHERIT:
#if defined(_SC_THREAD_PRIO_INHERIT)
      return sysconf(_SC_THREAD_PRIO_INHERIT);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_PRIO_INHERIT)
    case MUSL_SC_THREAD_PRIO_PROTECT:
#if defined(_SC_THREAD_PRIO_PROTECT)
      return sysconf(_SC_THREAD_PRIO_PROTECT);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_PRIO_PROTECT)
    case MUSL_SC_THREAD_PROCESS_SHARED:
#if defined(_SC_THREAD_PROCESS_SHARED)
      return sysconf(_SC_THREAD_PROCESS_SHARED);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_PROCESS_SHARED)
    case MUSL_SC_NPROCESSORS_CONF:
#if defined(_SC_NPROCESSORS_CONF)
      return sysconf(_SC_NPROCESSORS_CONF);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_NPROCESSORS_CONF)
    case MUSL_SC_NPROCESSORS_ONLN:
      return sysconf(_SC_NPROCESSORS_ONLN);
    case MUSL_SC_PHYS_PAGES:
      return sysconf(MUSL_SC_PHYS_PAGES);
    case MUSL_SC_ATEXIT_MAX:
#if defined(_SC_ATEXIT_MAX)
      return sysconf(_SC_ATEXIT_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_ATEXIT_MAX)
    case MUSL_SC_XOPEN_VERSION:
#if defined(_SC_XOPEN_VERSION)
      return sysconf(_SC_XOPEN_VERSION);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_VERSION)
    case MUSL_SC_XOPEN_UNIX:
#if defined(_SC_XOPEN_UNIX)
      return sysconf(_SC_XOPEN_UNIX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_UNIX)
    case MUSL_SC_XOPEN_CRYPT:
#if defined(_SC_XOPEN_CRYPT)
      return sysconf(_SC_XOPEN_CRYPT);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_CRYPT)
    case MUSL_SC_XOPEN_ENH_I18N:
#if defined(_SC_XOPEN_ENH_I18N)
      return sysconf(_SC_XOPEN_ENH_I18N);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_ENH_I18N)
    case MUSL_SC_XOPEN_SHM:
#if defined(_SC_XOPEN_SHM)
      return sysconf(_SC_XOPEN_SHM);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_SHM)
    case MUSL_SC_2_CHAR_TERM:
#if defined(_SC_2_CHAR_TERM)
      return sysconf(_SC_2_CHAR_TERM);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_CHAR_TERM)
    case MUSL_SC_2_UPE:
#if defined(_SC_2_UPE)
      return sysconf(_SC_2_UPE);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_UPE)
    // _XBS5_* options are recognized musl names, but bionic returns EINVAL
    // for them. Report -1 without EINVAL.
    case MUSL_SC_XBS5_ILP32_OFF32:
    case MUSL_SC_XBS5_ILP32_OFFBIG:
    case MUSL_SC_XBS5_LP64_OFF64:
    case MUSL_SC_XBS5_LPBIG_OFFBIG:
      return -1;
    case MUSL_SC_XOPEN_LEGACY:
#if defined(_SC_XOPEN_LEGACY)
      return sysconf(_SC_XOPEN_LEGACY);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_LEGACY)
    case MUSL_SC_XOPEN_REALTIME:
#if defined(_SC_XOPEN_REALTIME)
      return sysconf(_SC_XOPEN_REALTIME);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_REALTIME)
    case MUSL_SC_XOPEN_REALTIME_THREADS:
#if defined(_SC_XOPEN_REALTIME_THREADS)
      return sysconf(_SC_XOPEN_REALTIME_THREADS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_REALTIME_THREADS)
    case MUSL_SC_ADVISORY_INFO:
#if defined(_SC_ADVISORY_INFO)
      return sysconf(_SC_ADVISORY_INFO);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_ADVISORY_INFO)
    case MUSL_SC_BARRIERS:
#if defined(_SC_BARRIERS)
      return sysconf(_SC_BARRIERS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_BARRIERS)
    case MUSL_SC_CLOCK_SELECTION:
#if defined(_SC_CLOCK_SELECTION)
      return sysconf(_SC_CLOCK_SELECTION);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_CLOCK_SELECTION)
    case MUSL_SC_CPUTIME:
#if defined(_SC_CPUTIME)
      return sysconf(_SC_CPUTIME);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_CPUTIME)
    case MUSL_SC_THREAD_CPUTIME:
#if defined(_SC_THREAD_CPUTIME)
      return sysconf(_SC_THREAD_CPUTIME);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_CPUTIME)
    case MUSL_SC_MONOTONIC_CLOCK:
#if defined(_SC_MONOTONIC_CLOCK)
      return sysconf(_SC_MONOTONIC_CLOCK);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_MONOTONIC_CLOCK)
    case MUSL_SC_READER_WRITER_LOCKS:
#if defined(_SC_READER_WRITER_LOCKS)
      return sysconf(_SC_READER_WRITER_LOCKS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_READER_WRITER_LOCKS)
    case MUSL_SC_SPIN_LOCKS:
#if defined(_SC_SPIN_LOCKS)
      return sysconf(_SC_SPIN_LOCKS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SPIN_LOCKS)
    case MUSL_SC_REGEXP:
#if defined(_SC_REGEXP)
      return sysconf(_SC_REGEXP);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_REGEXP)
    case MUSL_SC_SHELL:
#if defined(_SC_SHELL)
      return sysconf(_SC_SHELL);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SHELL)
    case MUSL_SC_SPAWN:
#if defined(_SC_SPAWN)
      return sysconf(_SC_SPAWN);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SPAWN)
    case MUSL_SC_SPORADIC_SERVER:
#if defined(_SC_SPORADIC_SERVER)
      return sysconf(_SC_SPORADIC_SERVER);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SPORADIC_SERVER)
    case MUSL_SC_THREAD_SPORADIC_SERVER:
#if defined(_SC_THREAD_SPORADIC_SERVER)
      return sysconf(_SC_THREAD_SPORADIC_SERVER);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_SPORADIC_SERVER)
    case MUSL_SC_TIMEOUTS:
#if defined(_SC_TIMEOUTS)
      return sysconf(_SC_TIMEOUTS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TIMEOUTS)
    case MUSL_SC_TYPED_MEMORY_OBJECTS:
#if defined(_SC_TYPED_MEMORY_OBJECTS)
      return sysconf(_SC_TYPED_MEMORY_OBJECTS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TYPED_MEMORY_OBJECTS)
    case MUSL_SC_2_PBS:
#if defined(_SC_2_PBS)
      return sysconf(_SC_2_PBS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_PBS)
    case MUSL_SC_2_PBS_ACCOUNTING:
#if defined(_SC_2_PBS_ACCOUNTING)
      return sysconf(_SC_2_PBS_ACCOUNTING);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_PBS_ACCOUNTING)
    case MUSL_SC_2_PBS_LOCATE:
#if defined(_SC_2_PBS_LOCATE)
      return sysconf(_SC_2_PBS_LOCATE);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_PBS_LOCATE)
    case MUSL_SC_2_PBS_MESSAGE:
#if defined(_SC_2_PBS_MESSAGE)
      return sysconf(_SC_2_PBS_MESSAGE);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_PBS_MESSAGE)
    case MUSL_SC_2_PBS_TRACK:
#if defined(_SC_2_PBS_TRACK)
      return sysconf(_SC_2_PBS_TRACK);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_PBS_TRACK)
    case MUSL_SC_SYMLOOP_MAX:
#if defined(_SC_SYMLOOP_MAX)
      return sysconf(_SC_SYMLOOP_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SYMLOOP_MAX)
    case MUSL_SC_2_PBS_CHECKPOINT:
#if defined(_SC_2_PBS_CHECKPOINT)
      return sysconf(_SC_2_PBS_CHECKPOINT);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_2_PBS_CHECKPOINT)
    case MUSL_SC_V6_ILP32_OFF32:
#if defined(_SC_V6_ILP32_OFF32)
      return sysconf(_SC_V6_ILP32_OFF32);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V6_ILP32_OFF32)
    case MUSL_SC_V6_ILP32_OFFBIG:
#if defined(_SC_V6_ILP32_OFFBIG)
      return sysconf(_SC_V6_ILP32_OFFBIG);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V6_ILP32_OFFBIG)
    case MUSL_SC_V6_LP64_OFF64:
#if defined(_SC_V6_LP64_OFF64)
      return sysconf(_SC_V6_LP64_OFF64);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V6_LP64_OFF64)
    case MUSL_SC_V6_LPBIG_OFFBIG:
#if defined(_SC_V6_LPBIG_OFFBIG)
      return sysconf(_SC_V6_LPBIG_OFFBIG);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V6_LPBIG_OFFBIG)
    case MUSL_SC_HOST_NAME_MAX:
#if defined(_SC_HOST_NAME_MAX)
      return sysconf(_SC_HOST_NAME_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_HOST_NAME_MAX)
    case MUSL_SC_TRACE:
#if defined(_SC_TRACE)
      return sysconf(_SC_TRACE);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE)
    case MUSL_SC_TRACE_EVENT_FILTER:
#if defined(_SC_TRACE_EVENT_FILTER)
      return sysconf(_SC_TRACE_EVENT_FILTER);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE_EVENT_FILTER)
    case MUSL_SC_TRACE_INHERIT:
#if defined(_SC_TRACE_INHERIT)
      return sysconf(_SC_TRACE_INHERIT);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE_INHERIT)
    case MUSL_SC_TRACE_LOG:
#if defined(_SC_TRACE_LOG)
      return sysconf(_SC_TRACE_LOG);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE_LOG)
    case MUSL_SC_IPV6:
#if defined(_SC_IPV6)
      return sysconf(_SC_IPV6);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_IPV6)
    case MUSL_SC_RAW_SOCKETS:
#if defined(_SC_RAW_SOCKETS)
      return sysconf(_SC_RAW_SOCKETS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_RAW_SOCKETS)
    case MUSL_SC_V7_ILP32_OFF32:
#if defined(_SC_V7_ILP32_OFF32)
      return sysconf(_SC_V7_ILP32_OFF32);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V7_ILP32_OFF32)
    case MUSL_SC_V7_ILP32_OFFBIG:
#if defined(_SC_V7_ILP32_OFFBIG)
      return sysconf(_SC_V7_ILP32_OFFBIG);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V7_ILP32_OFFBIG)
    case MUSL_SC_V7_LP64_OFF64:
#if defined(_SC_V7_LP64_OFF64)
      return sysconf(_SC_V7_LP64_OFF64);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V7_LP64_OFF64)
    case MUSL_SC_V7_LPBIG_OFFBIG:
#if defined(_SC_V7_LPBIG_OFFBIG)
      return sysconf(_SC_V7_LPBIG_OFFBIG);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_V7_LPBIG_OFFBIG)
    case MUSL_SC_SS_REPL_MAX:
#if defined(_SC_SS_REPL_MAX)
      return sysconf(_SC_SS_REPL_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_SS_REPL_MAX)
    case MUSL_SC_TRACE_EVENT_NAME_MAX:
#if defined(_SC_TRACE_EVENT_NAME_MAX)
      return sysconf(_SC_TRACE_EVENT_NAME_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE_EVENT_NAME_MAX)
    case MUSL_SC_TRACE_NAME_MAX:
#if defined(_SC_TRACE_NAME_MAX)
      return sysconf(_SC_TRACE_NAME_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE_NAME_MAX)
    case MUSL_SC_TRACE_SYS_MAX:
#if defined(_SC_TRACE_SYS_MAX)
      return sysconf(_SC_TRACE_SYS_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE_SYS_MAX)
    case MUSL_SC_TRACE_USER_EVENT_MAX:
#if defined(_SC_TRACE_USER_EVENT_MAX)
      return sysconf(_SC_TRACE_USER_EVENT_MAX);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_TRACE_USER_EVENT_MAX)
    case MUSL_SC_XOPEN_STREAMS:
#if defined(_SC_XOPEN_STREAMS)
      return sysconf(_SC_XOPEN_STREAMS);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_XOPEN_STREAMS)
    case MUSL_SC_THREAD_ROBUST_PRIO_INHERIT:
#if defined(_SC_THREAD_ROBUST_PRIO_INHERIT)
      return sysconf(_SC_THREAD_ROBUST_PRIO_INHERIT);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif  // defined(_SC_THREAD_ROBUST_PRIO_INHERIT)
    case MUSL_SC_THREAD_ROBUST_PRIO_PROTECT:
#if defined(_SC_THREAD_ROBUST_PRIO_PROTECT)
      return sysconf(_SC_THREAD_ROBUST_PRIO_PROTECT);
#else
      return -1;  // valid name, unsupported: no EINVAL
#endif        // defined(_SC_THREAD_ROBUST_PRIO_PROTECT)
    default:  // Explicitly handle unsupported names
      errno = EINVAL;
      return -1;
  }
}

// If |musl_conf_to_platform_conf| returns -1,
// just return -1 (errno is set to EINVAL by musl_conf_to_platform_conf()).
long __abi_wrap_pathconf(const char* path, int name) {
  int converted_name = musl_conf_to_platform_conf(name);
  if (converted_name == kMuslConfUnsupported) {
    return -1;
  }
  if (converted_name == -1) {
    return -1;
  }
  int saved_errno = errno;
  errno = 0;
  long result = pathconf(path, converted_name);
  if (result == -1 && (errno == EINVAL || errno == 0)) {
    // Platform defines the constant (e.g. _PC_REC_*_XFER_SIZE) but doesn't
    // provide a value for it. POSIX allows returning -1 with errno unchanged
    // to mean "no limit", so restore the caller's original errno.
    errno = saved_errno;
  }
  return result;
}

musl_uid_t __abi_wrap_geteuid() {
  return static_cast<musl_uid_t>(geteuid());
}

musl_uid_t __abi_wrap_getuid() {
  return static_cast<musl_uid_t>(getuid());
}

musl_pid_t __abi_wrap_getpid() {
  return static_cast<musl_pid_t>(getpid());
}

musl_pid_t __abi_wrap_gettid() {
#if defined(gettid)
  return static_cast<musl_pid_t>(gettid());
#else
  return static_cast<musl_pid_t>(syscall(SYS_gettid));
#endif
}

int __abi_wrap_access(const char* path, int amode) {
  return access(path, access_helper(amode));
}

int __abi_wrap_fchown(int fd, musl_uid_t owner, musl_gid_t group) {
  return fchown(fd, static_cast<uid_t>(owner), static_cast<gid_t>(group));
}

int __abi_wrap_link(const char* path1, const char* path2) {
#if defined(ANDROID)
  // Mimicking Bionic's implementation
  return linkat(AT_FDCWD, path1, AT_FDCWD, path2, 0);
#endif
  return link(path1, path2);
}

int __abi_wrap_unlinkat(int fildes, const char* path, int musl_flag) {
  fildes = (fildes == MUSL_AT_FDCWD) ? AT_FDCWD : fildes;
  int flag = musl_unlink_flag_to_platform_flag(musl_flag);
  if (flag == -1) {
    return -1;
  }
  return unlinkat(fildes, path, flag);
}

ssize_t __abi_wrap_pread(int fd, void* buf, size_t size, musl_off_t ofs) {
  return pread(fd, buf, size, static_cast<off_t>(ofs));
}

ssize_t __abi_wrap_pwrite(int fd,
                          const void* buf,
                          size_t size,
                          musl_off_t ofs) {
  return pwrite(fd, buf, size, static_cast<off_t>(ofs));
}

ssize_t __abi_wrap_readlink(const char* path, char* buf, size_t bufsize) {
#if defined(__ANDROID__)
  // Passing a bufsize that wraps around size_t triggers a 
  // FORTIFY arithmetic overflow abort in bionic
  if (bufsize == 0 || bufsize > SSIZE_MAX) {
    errno = EINVAL;
    return -1;
  }
#endif  // defined(__ANDROID__)
  return readlink(path, buf, bufsize);
}

ssize_t __abi_wrap_readlinkat(int dirfd,
                              const char* path,
                              char* buf,
                              size_t bufsize) {
#if defined(__ANDROID__)
  if (bufsize == 0 || bufsize > SSIZE_MAX) {
    errno = EINVAL;
    return -1;
  }
#endif  // defined(__ANDROID__)
  return readlinkat(dirfd, path, buf, bufsize);
}
