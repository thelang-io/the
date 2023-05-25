/*!
 * Copyright (c) Aaron Delasy
 * Licensed under the MIT License
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define EOL "\n"

typedef void *(*threadpool_func_t) (void *, void *);

typedef struct threadpool_job {
  threadpool_func_t func;
  void *ctx;
  struct threadpool_job *next;
} threadpool_job_t;

typedef struct threadpool_thread {
  pthread_t id;
  struct threadpool_thread *next;
} threadpool_thread_t;

typedef struct threadpool {
  bool active;
  pthread_cond_t cond1;
  pthread_cond_t cond2;
  threadpool_job_t *jobs;
  pthread_mutex_t lock;
  threadpool_thread_t *threads;
  int working_threads;
} *threadpool_t;

threadpool_t threadpool_init (int);
void threadpool_deinit (threadpool_t);
void threadpool_add (threadpool_t, threadpool_func_t, void *);
void threadpool_add_and_wait (threadpool_t, threadpool_func_t, void *);
threadpool_job_t *threadpool_next (threadpool_t);
void threadpool_wait (threadpool_t);
void *threadpool_worker (void *);
void threadpool_job_deinit (threadpool_job_t *);
threadpool_thread_t *threadpool_thread_init (threadpool_t, threadpool_thread_t *);
void threadpool_thread_deinit (threadpool_thread_t *);

threadpool_t threadpool_init (int count) {
  threadpool_t self = malloc(sizeof(struct threadpool));

  self->active = true;
  pthread_cond_init(&self->cond1, NULL);
  pthread_cond_init(&self->cond2, NULL);
  self->jobs = NULL;
  pthread_mutex_init(&self->lock, NULL);
  self->threads = NULL;
  self->working_threads = 0;

  for (int i = 0; i < count; i++) {
    self->threads = threadpool_thread_init(self, self->threads);
  }

  return self;
}

void threadpool_deinit (threadpool_t self) {
  pthread_mutex_lock(&self->lock);
  threadpool_job_t *it1 = self->jobs;

  while (it1 != NULL) {
    threadpool_job_t *tmp = it1->next;
    threadpool_job_deinit(it1);
    it1 = tmp;
  }

  self->active = false;
  pthread_cond_broadcast(&self->cond1);
  pthread_mutex_unlock(&self->lock);
  threadpool_wait(self);

  threadpool_thread_t *it2 = self->threads;

  while (it2 != NULL) {
    threadpool_thread_t *tmp = it2->next;
    threadpool_thread_deinit(it2);
    it2 = tmp;
  }

  pthread_cond_destroy(&self->cond1);
  pthread_cond_destroy(&self->cond2);
  pthread_mutex_destroy(&self->lock);
  free(self);
}

void threadpool_add (threadpool_t self, threadpool_func_t func, void *ctx) {
  threadpool_job_t *job = malloc(sizeof(threadpool_job_t));

  job->func = func;
  job->ctx = ctx;
  job->next = NULL;
  pthread_mutex_lock(&self->lock);

  if (self->jobs == NULL) {
    self->jobs = job;
  } else {
    threadpool_job_t *tail = self->jobs;

    while (tail->next != NULL) {
      tail = tail->next;
    }

    tail->next = job;
  }

  pthread_cond_broadcast(&self->cond1);
  pthread_mutex_unlock(&self->lock);
}

void threadpool_add_and_wait (threadpool_t self, threadpool_func_t func, void *ctx) {
  // todo add job, assign unique id to it
  // todo check if popped job has id that you are waiting for
}

threadpool_job_t *threadpool_next (threadpool_t self) {
  threadpool_job_t *job = self->jobs;

  if (self->jobs != NULL) {
    self->jobs = self->jobs->next;
  }

  return job;
}

void *threadpool_worker (void *params) {
  threadpool_t self = params;

  while (1) {
    pthread_mutex_lock(&self->lock);

    while (self->jobs == NULL && self->active) {
      pthread_cond_wait(&self->cond1, &self->lock);
    }

    if (!self->active) {
      pthread_cond_signal(&self->cond2);
      pthread_mutex_unlock(&self->lock);
      pthread_exit(NULL);
    }

    threadpool_job_t *job = threadpool_next(self);
    self->working_threads++;
    pthread_mutex_unlock(&self->lock);

    if (job != NULL) {
      job->func(self, job->ctx);
      threadpool_job_deinit(job);
    }

    pthread_mutex_lock(&self->lock);
    self->working_threads--;

    if (self->active && self->jobs == NULL) {
      pthread_cond_signal(&self->cond2);
    }

    pthread_mutex_unlock(&self->lock);
  }
}

void threadpool_wait (threadpool_t self) {
  pthread_mutex_lock(&self->lock);

  while (1) {
    if (self->jobs != NULL || self->working_threads != 0) {
      pthread_cond_wait(&self->cond2, &self->lock);
    } else {
      break;
    }
  }

  pthread_mutex_unlock(&self->lock);
}

void threadpool_job_deinit (threadpool_job_t *self) {
  free(self);
}

threadpool_thread_t *threadpool_thread_init (threadpool_t tp, threadpool_thread_t *next) {
  threadpool_thread_t *self = malloc(sizeof(threadpool_thread_t));

  pthread_create(&self->id, NULL, threadpool_worker, tp);
  pthread_detach(self->id);
  self->next = next;

  return self;
}

void threadpool_thread_deinit (threadpool_thread_t *self) {
  free(self);
}

void await (int ms) {
  usleep(ms * 1e3);
}

unsigned long long date_now () {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
}

void *test2 (__unused void *_tp, __unused void *_p) {
  printf("Started2" EOL);
  await(1000);
  printf("Done2" EOL);
  return NULL;
}

void *test (void *_tp, __unused void *_p) {
  threadpool_t tp = _tp;
  printf("Started" EOL);
  await(1000);
  threadpool_add_and_wait(tp, test2, NULL);
  printf("Done" EOL);
  return NULL;
}

int main () {
  threadpool_t tp = threadpool_init(4);

  printf("%llu" EOL, date_now());
  threadpool_add(tp, test, NULL);
  printf("Hello, World!" EOL);
  threadpool_wait(tp);
  printf("%llu" EOL, date_now());

  threadpool_deinit(tp);
}
