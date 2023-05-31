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

// todo test thread_id

const std::vector<std::string> codegenThread = {
  R"(void thread_sleep (_{int32_t} i) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{Sleep}((unsigned int) i);)" EOL
  R"(  #else)" EOL
  R"(    _{usleep}((unsigned int) (i * 1000));)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(_{uint64_t} thread_id () {)" EOL
  R"(  _{uint64_t} id;)" EOL
  R"(  _{pthread_threadid_np}(_{NULL}, &id);)" EOL
  R"(  return id;)" EOL
  R"(})" EOL,

  R"(_{threadpool_t} *threadpool_init (int count) {)" EOL
  R"(  _{threadpool_t} *self = _{alloc}(sizeof(_{threadpool_t}));)" EOL
  R"(  self->active = _{true};)" EOL
  R"(  _{pthread_cond_init}(&self->cond1, _{NULL});)" EOL
  R"(  _{pthread_cond_init}(&self->cond2, _{NULL});)" EOL
  R"(  self->jobs = _{NULL};)" EOL
  R"(  _{pthread_mutex_init}(&self->lock, _{NULL});)" EOL
  R"(  self->threads = _{NULL};)" EOL
  R"(  self->working_threads = 0;)" EOL
  R"(  for (int i = 0; i < count; i++) self->threads = _{threadpool_thread_init}(self, self->threads);)" EOL
  R"(  return self;)" EOL
  R"(})" EOL,

  R"(void threadpool_deinit (_{threadpool_t} *self) {)" EOL
  R"(  _{pthread_mutex_lock}(&self->lock);)" EOL
  R"(  _{threadpool_job_t} *it1 = self->jobs;)" EOL
  R"(  while (it1 != _{NULL}) {)" EOL
  R"(    _{threadpool_job_t} *tmp = it1->next;)" EOL
  R"(    _{threadpool_job_deinit}(it1);)" EOL
  R"(    it1 = tmp;)" EOL
  R"(  })" EOL
  R"(  self->active = _{false};)" EOL
  R"(  _{pthread_cond_broadcast}(&self->cond1);)" EOL
  R"(  _{pthread_mutex_unlock}(&self->lock);)" EOL
  R"(  _{threadpool_wait}(self);)" EOL
  R"(  _{threadpool_thread_t} *it2 = self->threads;)" EOL
  R"(  while (it2 != _{NULL}) {)" EOL
  R"(    _{threadpool_thread_t} *tmp = it2->next;)" EOL
  R"(    _{threadpool_thread_deinit}(it2);)" EOL
  R"(    it2 = tmp;)" EOL
  R"(  })" EOL
  R"(  _{pthread_cond_destroy}(&self->cond1);)" EOL
  R"(  _{pthread_cond_destroy}(&self->cond2);)" EOL
  R"(  _{pthread_mutex_destroy}(&self->lock);)" EOL
  R"(  _{free}(self);)" EOL
  R"(})" EOL,

  R"(void threadpool_add (_{threadpool_t} *self, _{threadpool_func_t} func, void *ctx, void *params, _{threadpool_job_t} *parent) {)" EOL
  R"(  _{threadpool_job_t} *job = _{alloc}(sizeof(_{threadpool_job_t}));)" EOL
  R"(  job->parent = parent;)" EOL
  R"(  job->func = func;)" EOL
  R"(  job->ctx = ctx;)" EOL
  R"(  job->params = params;)" EOL
  R"(  job->step = 0;)" EOL
  R"(  job->referenced = _{false};)" EOL
  R"(  job->next = _{NULL};)" EOL
  R"(  _{threadpool_insert}(self, job);)" EOL
  R"(})" EOL,

  R"(void threadpool_insert (_{threadpool_t} *self, _{threadpool_job_t} *job) {)" EOL
  R"(  _{pthread_mutex_lock}(&self->lock);)" EOL
  R"(  if (self->jobs == _{NULL}) {)" EOL
  R"(    self->jobs = job;)" EOL
  R"(  } else {)" EOL
  R"(    _{threadpool_job_t} *tail = self->jobs;)" EOL
  R"(    while (tail->next != _{NULL}) tail = tail->next;)" EOL
  R"(    tail->next = job;)" EOL
  R"(  })" EOL
  R"(  _{pthread_cond_broadcast}(&self->cond1);)" EOL
  R"(  _{pthread_mutex_unlock}(&self->lock);)" EOL
  R"(})" EOL,

  R"(void *threadpool_worker (void *n) {)" EOL
  R"(  _{threadpool_t} *self = n;)" EOL
  R"(  while (1) {)" EOL
  R"(    _{pthread_mutex_lock}(&self->lock);)" EOL
  R"(    while (self->active && self->jobs == _{NULL}) _{pthread_cond_wait}(&self->cond1, &self->lock);)" EOL
  R"(    if (!self->active) {)" EOL
  R"(      _{pthread_cond_signal}(&self->cond2);)" EOL
  R"(      _{pthread_mutex_unlock}(&self->lock);)" EOL
  R"(      _{pthread_exit}(_{NULL});)" EOL
  R"(    })" EOL
  R"(    _{threadpool_job_t} *job = self->jobs;)" EOL
  R"(    self->jobs = self->jobs->next;)" EOL
  R"(    self->working_threads++;)" EOL
  R"(    _{pthread_mutex_unlock}(&self->lock);)" EOL
  R"(    if (job != _{NULL}) {)" EOL
  R"(      int step = job->step;)" EOL
  R"(      job->step++;)" EOL
  R"(      job->func(self, job, job->ctx, job->params, step);)" EOL
  R"(      if (job->parent != _{NULL} && !job->referenced) _{threadpool_insert}(self, job->parent);)" EOL
  R"(      _{threadpool_job_deinit}(job);)" EOL
  R"(    })" EOL
  R"(    _{pthread_mutex_lock}(&self->lock);)" EOL
  R"(    self->working_threads--;)" EOL
  R"(    if (self->active && self->jobs == _{NULL}) _{pthread_cond_signal}(&self->cond2);)" EOL
  R"(    _{pthread_mutex_unlock}(&self->lock);)" EOL
  R"(  })" EOL
  R"(})" EOL,

  R"(void threadpool_wait (_{threadpool_t} *self) {)" EOL
  R"(  _{pthread_mutex_lock}(&self->lock);)" EOL
  R"(  while (self->working_threads != 0 || self->jobs != _{NULL}) _{pthread_cond_wait}(&self->cond2, &self->lock);)" EOL
  R"(  _{pthread_mutex_unlock}(&self->lock);)" EOL
  R"(})" EOL,

  R"(void threadpool_job_deinit (_{threadpool_job_t} *self) {)" EOL
  R"(  _{free}(self);)" EOL
  R"(})" EOL,

  R"(_{threadpool_job_t} *threadpool_job_ref (_{threadpool_job_t} *self) {)" EOL
  R"(  _{threadpool_job_t} *r = _{alloc}(sizeof(_{threadpool_job_t}));)" EOL
  R"(  r->parent = self->parent;)" EOL
  R"(  r->func = self->func;)" EOL
  R"(  r->ctx = self->ctx;)" EOL
  R"(  r->params = self->params;)" EOL
  R"(  r->step = self->step;)" EOL
  R"(  r->referenced = self->referenced;)" EOL
  R"(  r->next = _{NULL};)" EOL
  R"(  self->referenced = _{true};)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{threadpool_thread_t} *threadpool_thread_init (_{threadpool_t} *tp, _{threadpool_thread_t} *next) {)" EOL
  R"(  _{threadpool_thread_t} *self = _{alloc}(sizeof(_{threadpool_thread_t}));)" EOL
  R"(  _{pthread_create}(&self->id, _{NULL}, _{threadpool_worker}, tp);)" EOL
  R"(  _{pthread_detach}(self->id);)" EOL
  R"(  self->next = next;)" EOL
  R"(  return self;)" EOL
  R"(})" EOL,

  R"(void threadpool_thread_deinit (_{threadpool_thread_t} *self) {)" EOL
  R"(  _{free}(self);)" EOL
  R"(})" EOL
};
