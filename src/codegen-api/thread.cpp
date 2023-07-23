/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "thread.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenThread = {
  R"(_{int32_t} thread_id () {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    return (_{int32_t}) _{GetCurrentThreadId}();)" EOL
  R"(  #elif defined(_{THE_OS_MACOS}))" EOL
  R"(    _{uint64_t} id;)" EOL
  R"(    _{pthread_threadid_np}(_{NULL}, &id);)" EOL
  R"(    return (_{int32_t}) id;)" EOL
  R"(  #else)" EOL
  R"(    return (_{int32_t}) _{syscall}(_{SYS_gettid});)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void thread_sleep (_{int32_t} i) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{Sleep}((unsigned int) i);)" EOL
  R"(  #else)" EOL
  R"(    _{usleep}((unsigned int) (i * 1000));)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(_{threadpool_t} *threadpool_init (int count) {)" EOL
  R"(  _{threadpool_t} *self = _{alloc}(sizeof(_{threadpool_t}));)" EOL
  R"(  self->active = _{true};)" EOL
  R"(  self->jobs = _{NULL};)" EOL
  R"(  self->jobs_tail = _{NULL};)" EOL
  R"(  self->working_threads = 0;)" EOL
  R"(  self->alive_threads = 0;)" EOL
  R"(  _{threadpool_cond_init}(&self->cond1);)" EOL
  R"(  _{threadpool_cond_init}(&self->cond2);)" EOL
  R"(  _{threadpool_mutex_init}(&self->lock1);)" EOL
  R"(  _{threadpool_mutex_init}(&self->lock2);)" EOL
  R"(  _{threadpool_mutex_init}(&self->lock3);)" EOL
  R"(  self->threads = _{NULL};)" EOL
  R"(  for (int i = 0; i < count; i++) self->threads = _{threadpool_thread_init}(self, self->threads);)" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(  while (self->alive_threads != count) _{threadpool_cond_wait}(&self->cond1, &self->lock1);)" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(  return self;)" EOL
  R"(})" EOL,

  R"(void threadpool_deinit (_{threadpool_t} *self) {)" EOL
  R"(  self->active = _{false};)" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(  while (self->alive_threads != 0) {)" EOL
  R"(    _{threadpool_mutex_lock}(&self->lock3);)" EOL
  R"(    _{threadpool_cond_signal}(&self->cond2);)" EOL
  R"(    _{threadpool_mutex_unlock}(&self->lock3);)" EOL
  R"(    _{threadpool_cond_wait}(&self->cond1, &self->lock1);)" EOL
  R"(  })" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(  while (self->jobs != _{NULL}) {)" EOL
  R"(    _{threadpool_job_t} *next = self->jobs->next;)" EOL
  R"(    _{threadpool_job_deinit}(self->jobs);)" EOL
  R"(    self->jobs = next;)" EOL
  R"(  })" EOL
  R"(  while (self->threads != _{NULL}) {)" EOL
  R"(    _{threadpool_thread_t} *next = self->threads->next;)" EOL
  R"(    _{threadpool_thread_deinit}(self->threads);)" EOL
  R"(    self->threads = next;)" EOL
  R"(  })" EOL
  R"(  _{threadpool_cond_deinit}(&self->cond1);)" EOL
  R"(  _{threadpool_cond_deinit}(&self->cond2);)" EOL
  R"(  _{threadpool_mutex_deinit}(&self->lock1);)" EOL
  R"(  _{threadpool_mutex_deinit}(&self->lock2);)" EOL
  R"(  _{threadpool_mutex_deinit}(&self->lock3);)" EOL
  R"(  _{free}(self);)" EOL
  R"(})" EOL,

  R"(void threadpool_add (_{threadpool_t} *self, _{threadpool_func_t} func, void *ctx, void *params, void *ret, _{threadpool_job_t} *parent) {)" EOL
  R"(  _{threadpool_job_t} *job = _{alloc}(sizeof(_{threadpool_job_t}));)" EOL
  R"(  job->parent = parent;)" EOL
  R"(  job->func = func;)" EOL
  R"(  job->ctx = ctx;)" EOL
  R"(  job->params = params;)" EOL
  R"(  job->ret = ret;)" EOL
  R"(  job->step = 0;)" EOL
  R"(  job->init = 0;)" EOL
  R"(  job->next = _{NULL};)" EOL
  R"(  _{threadpool_insert}(self, job);)" EOL
  R"(})" EOL,

  R"(void threadpool_cond_init (_{threadpool_cond_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{InitializeConditionVariable}(&self->v);)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_cond_init}(&self->v, _{NULL});)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void threadpool_cond_deinit (_{threadpool_cond_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    (void) &self;)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_cond_destroy}(&self->v);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void threadpool_cond_signal (_{threadpool_cond_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{WakeConditionVariable}(&self->v);)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_cond_signal}(&self->v);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void threadpool_cond_wait (_{threadpool_cond_t} *self, _{threadpool_mutex_t} *mutex) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{SleepConditionVariableCS}(&self->v, &mutex->v, _{INFINITE});)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_cond_wait}(&self->v, &mutex->v);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void threadpool_error_assign (_{threadpool_t} *self, _{err_state_t} *fn_err_state) {)" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(  _{memcpy}(&_{err_state}, fn_err_state, sizeof(_{err_state_t}));)" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(})" EOL,

  R"(void threadpool_error_assign_parent (_{threadpool_t} *self, _{threadpool_job_t} *job, _{err_state_t} *fn_err_state) {)" EOL
  R"(  _{memcpy}(*((void **) job->params), fn_err_state, sizeof(_{err_state_t}));)" EOL
  R"(})" EOL,

  R"(_{threadpool_job_t} *threadpool_get (_{threadpool_t} *self) {)" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock2);)" EOL
  R"(  _{threadpool_job_t} *job = self->jobs;)" EOL
  R"(  if (job != _{NULL}) {)" EOL
  R"(    self->jobs = self->jobs->next;)" EOL
  R"(    job->next = _{NULL};)" EOL
  R"(    if (self->jobs == _{NULL}) {)" EOL
  R"(      self->jobs_tail = _{NULL};)" EOL
  R"(    } else {)" EOL
  R"(      _{threadpool_mutex_lock}(&self->lock3);)" EOL
  R"(      _{threadpool_cond_signal}(&self->cond2);)" EOL
  R"(      _{threadpool_mutex_unlock}(&self->lock3);)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock2);)" EOL
  R"(  return job;)" EOL
  R"(})" EOL,

  R"(void threadpool_insert (_{threadpool_t} *self, _{threadpool_job_t} *job) {)" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock2);)" EOL
  R"(  if (self->jobs == _{NULL}) {)" EOL
  R"(    self->jobs = job;)" EOL
  R"(    self->jobs_tail = job;)" EOL
  R"(  } else {)" EOL
  R"(    self->jobs_tail->next = job;)" EOL
  R"(    self->jobs_tail = self->jobs_tail->next;)" EOL
  R"(  })" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock3);)" EOL
  R"(  _{threadpool_cond_signal}(&self->cond2);)" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock3);)" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock2);)" EOL
  R"(})" EOL,

  R"(void *threadpool_worker (void *n) {)" EOL
  R"(  _{threadpool_thread_t} *thread = n;)" EOL
  R"(  _{threadpool_t} *self = thread->tp;)" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(  self->alive_threads++;)" EOL
  R"(  _{threadpool_cond_signal}(&self->cond1);)" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(  while (self->active) {)" EOL
  R"(    _{threadpool_mutex_lock}(&self->lock3);)" EOL
  R"(    while (self->active && self->jobs == _{NULL}) _{threadpool_cond_wait}(&self->cond2, &self->lock3);)" EOL
  R"(    _{threadpool_mutex_unlock}(&self->lock3);)" EOL
  R"(    if (!self->active) break;)" EOL
  R"(    _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(    self->working_threads++;)" EOL
  R"(    _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(    _{threadpool_job_t} *job = _{threadpool_get}(self);)" EOL
  R"(    if (job != _{NULL}) job->func(self, job, job->ctx, job->params, job->ret, job->step);)" EOL
  R"(    _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(    self->working_threads--;)" EOL
  R"(    if ((self->working_threads == 0 && self->jobs == _{NULL}) || _{err_state}.id != -1) _{threadpool_cond_signal}(&self->cond1);)" EOL
  R"(    _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(  })" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(  self->alive_threads--;)" EOL
  R"(  _{threadpool_cond_signal}(&self->cond1);)" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(  return _{NULL};)" EOL
  R"(})" EOL,

  R"(void threadpool_wait (_{threadpool_t} *self) {)" EOL
  R"(  _{threadpool_mutex_lock}(&self->lock1);)" EOL
  R"(  while ((self->working_threads != 0 || self->jobs != _{NULL}) && _{err_state}.id == -1) {)" EOL
  R"(    _{threadpool_cond_wait}(&self->cond1, &self->lock1);)" EOL
  R"(  })" EOL
  R"(  _{threadpool_mutex_unlock}(&self->lock1);)" EOL
  R"(})" EOL,

  R"(void threadpool_job_deinit (_{threadpool_job_t} *self) {)" EOL
  R"(  if (self->ctx != _{NULL}) _{free}(self->ctx);)" EOL
  R"(  _{free}(self);)" EOL
  R"(})" EOL,

  R"(_{threadpool_job_t} *threadpool_job_step (_{threadpool_job_t} *self, int step) {)" EOL
  R"(  self->step = step;)" EOL
  R"(  return self;)" EOL
  R"(})" EOL,

  R"(void threadpool_mutex_init (_{threadpool_mutex_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{InitializeCriticalSection}(&self->v);)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_mutex_init}(&self->v, _{NULL});)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void threadpool_mutex_deinit (_{threadpool_mutex_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{DeleteCriticalSection}(&self->v);)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_mutex_destroy}(&self->v);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void threadpool_mutex_lock (_{threadpool_mutex_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{EnterCriticalSection}(&self->v);)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_mutex_lock}(&self->v);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void threadpool_mutex_unlock (_{threadpool_mutex_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{LeaveCriticalSection}(&self->v);)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_mutex_unlock}(&self->v);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(_{threadpool_thread_t} *threadpool_thread_init (_{threadpool_t} *tp, _{threadpool_thread_t} *next) {)" EOL
  R"(  _{threadpool_thread_t} *self = _{alloc}(sizeof(_{threadpool_thread_t}));)" EOL
  R"(  self->tp = tp;)" EOL
  R"(  self->next = next;)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    self->id = _{CreateThread}(_{NULL}, 0, (_{LPTHREAD_START_ROUTINE}) _{threadpool_worker}, self, _{CREATE_SUSPENDED}, _{NULL});)" EOL
  R"(    _{ResumeThread}(self->id);)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_create}(&self->id, _{NULL}, _{threadpool_worker}, self);)" EOL
  R"(  #endif)" EOL
  R"(  return self;)" EOL
  R"(})" EOL,

  R"(void threadpool_thread_deinit (_{threadpool_thread_t} *self) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{WaitForSingleObject}(self->id, _{INFINITE});)" EOL
  R"(    _{CloseHandle}(self->id);)" EOL
  R"(    _{MemoryBarrier}();)" EOL
  R"(  #else)" EOL
  R"(    _{pthread_join}(self->id, _{NULL});)" EOL
  R"(  #endif)" EOL
  R"(  _{free}(self);)" EOL
  R"(})" EOL
};
