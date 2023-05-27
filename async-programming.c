/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define EOL "\n"

typedef struct {
  int testVar;
} ctx_t;

typedef struct {
  void *ctx;
  void *params;
  int step;
  void *ret;
} data_t;

struct fn0P {
  int n0;
};

typedef void *(*threadpool_func_t) (void *, data_t);

typedef struct threadpool_job {
  unsigned int id;
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
  unsigned int last_job;
  pthread_mutex_t lock;
  threadpool_thread_t *threads;
  int working_threads;
} *threadpool_t;

threadpool_t threadpool_init (int);
void threadpool_deinit (threadpool_t);
unsigned int threadpool_add (threadpool_t, threadpool_func_t, void *);
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
  self->last_job = 1;
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

unsigned int threadpool_add (threadpool_t self, threadpool_func_t func, void *params) {
  threadpool_job_t *job = malloc(sizeof(threadpool_job_t));

  if (self->last_job == UINT_MAX) {
    job->id = self->last_job;
    self->last_job = 0;
  } else {
    job->id = self->last_job++;
  }

  job->func = func;
  job->params = params;
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
  return job->id;
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
      job->func(self, (data_t) {job->ctx});
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

unsigned long long date_now () {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
}

void *await (void *_tp, data_t data) {
  struct fn0P *p = (struct fn0P *) data.params;
  int ms = p->n0;
  usleep(ms * 1e3);
  return NULL;
}

void *test2 (void *_tp, data_t data) {
  threadpool_t tp = _tp;
  ctx_t *ctx = data.ctx;
  int *testVar = &ctx->testVar;

  switch (data.step) {
    case 0: {
      *testVar = 1000;
      printf("Started2" EOL);
      threadpool_add(tp, await, (void *) &(struct fn0P) {*testVar});
      data.step = 1;
      break;
    }
    case 1: {
      printf("Var: %d" EOL, *testVar);
      printf("Done2" EOL);
      break;
    }
  }

  return NULL;
}

void *test (void *_tp, data_t data) {
  threadpool_t tp = _tp;

  switch (data.step) {
    case 0: {
      printf("Started" EOL);
      threadpool_add(tp, await, (void *) &(struct fn0P) {1000});
      break;
    }
    case 1: {
      threadpool_add(tp, test2, NULL);
      break;
    }
    case 2: {
      printf("Done" EOL);
      break;
    }
  }

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
