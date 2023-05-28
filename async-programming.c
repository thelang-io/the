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

typedef struct {
  int testVar;
} ctx_t;

typedef struct {
  void *ctx;
  void *params;
  int step;
} data_t;

struct fn0P {
  int n0;
};

struct threadpool_job;
typedef void (*threadpool_func_t) (void *, struct threadpool_job *, data_t);

typedef struct threadpool_job {
  struct threadpool_job *parent;
  threadpool_func_t func;
  void *ctx;
  void *params;
  int step;
  bool referenced;
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
void threadpool_add (threadpool_t, threadpool_func_t, void *, void *, threadpool_job_t *);
void threadpool_insert (threadpool_t, threadpool_job_t *);
void threadpool_wait (threadpool_t);
void *threadpool_worker (void *);
void threadpool_job_deinit (threadpool_job_t *);
threadpool_job_t *threadpool_job_ref (threadpool_job_t *);
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

void threadpool_add (
  threadpool_t self,
  threadpool_func_t func,
  void *ctx,
  void *params,
  threadpool_job_t *parent
) {
  threadpool_job_t *job = malloc(sizeof(threadpool_job_t));

  job->parent = parent;
  job->func = func;
  job->ctx = ctx;
  job->params = params;
  job->step = 0;
  job->referenced = false;
  job->next = NULL;

  threadpool_insert(self, job);
}

void threadpool_insert (threadpool_t self, threadpool_job_t *job) {
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

void *threadpool_worker (void *params) {
  threadpool_t self = params;

  while (1) {
    pthread_mutex_lock(&self->lock);

    while (self->active && self->jobs == NULL) {
      pthread_cond_wait(&self->cond1, &self->lock);
    }

    if (!self->active) {
      pthread_cond_signal(&self->cond2);
      pthread_mutex_unlock(&self->lock);
      pthread_exit(NULL);
    }

    threadpool_job_t *job = self->jobs;
    self->jobs = self->jobs->next;
    self->working_threads++;
    pthread_mutex_unlock(&self->lock);

    if (job != NULL) {
      int step = job->step;
      job->step++;
      job->func(self, job, (data_t) {job->ctx, job->params, step});

      if (job->parent != NULL && !job->referenced) {
        threadpool_insert(self, job->parent);
      }

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
    if (self->working_threads != 0 || self->jobs != NULL) {
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

threadpool_job_t *threadpool_job_ref (threadpool_job_t *self) {
  threadpool_job_t *r = malloc(sizeof(threadpool_job_t));

  r->parent = self->parent;
  r->func = self->func;
  r->ctx = self->ctx;
  r->params = self->params;
  r->step = self->step;
  r->referenced = self->referenced;
  r->next = NULL;
  self->referenced = true;

  return r;
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

ctx_t *ctx_alloc () {
  return malloc(sizeof(ctx_t));
}

struct fn0P *fn0P_alloc (int n0) {
  struct fn0P *r = malloc(sizeof(struct fn0P));
  r->n0 = n0;
  return r;
}

unsigned long long thread_id () {
  unsigned long long id;
  pthread_threadid_np(NULL, &id);
  return id;
}

unsigned long long date_now () {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec * 1000 + ts.tv_nsec / 1000;
}

void await (__unused void *_tp, __unused threadpool_job_t *self, data_t data) {
  struct fn0P *p = (struct fn0P *) data.params;
  int ms = p->n0;
  printf("Thread-%llu: AWAIT: %d" EOL, thread_id(), ms);
  usleep(ms * 1000);
}

void test3 (void *_tp, threadpool_job_t *self, data_t data) {
  threadpool_t tp = _tp;

  switch (data.step) {
    case 0: {
      printf("Thread-%llu: START_3" EOL, thread_id());
      threadpool_add(tp, await, data.ctx, fn0P_alloc(1000), threadpool_job_ref(self));
      break;
    }
    case 1: {
      printf("Thread-%llu: END_3" EOL, thread_id());
    }
  }
}

void test2 (void *_tp, threadpool_job_t *self, data_t data) {
  threadpool_t tp = _tp;
  ctx_t *ctx = data.ctx;
  int *testVar = &ctx->testVar;

  switch (data.step) {
    case 0: {
      printf("Thread-%llu: START_2" EOL, thread_id());
      *testVar = 1000;
      threadpool_add(tp, await, data.ctx, fn0P_alloc(*testVar), threadpool_job_ref(self));
      break;
    }
    case 1: {
      printf("Thread-%llu: END_2" EOL, thread_id());
    }
  }
}

void test1 (void *_tp, threadpool_job_t *self, data_t data) {
  threadpool_t tp = _tp;

  switch (data.step) {
    case 0: {
      printf("Thread-%llu: START_1" EOL, thread_id());
      threadpool_add(tp, await, data.ctx, fn0P_alloc(1000), threadpool_job_ref(self));
      break;
    }
    case 1: {
      printf("Thread-%llu: MIDDLE_1" EOL, thread_id());
      threadpool_add(tp, test3, data.ctx, NULL, NULL);
      threadpool_add(tp, test2, data.ctx, NULL, threadpool_job_ref(self));
      break;
    }
    case 2: {
      printf("Thread-%llu: END_1" EOL, thread_id());
    }
  }
}

int main () {
  threadpool_t tp = threadpool_init(6);

  printf("Thread-%llu: %llu" EOL, thread_id(), date_now());
  threadpool_add(tp, test1, ctx_alloc(), NULL, NULL);
  threadpool_wait(tp);
  printf("Thread-%llu: %llu" EOL, thread_id(), date_now());

  threadpool_deinit(tp);
}
