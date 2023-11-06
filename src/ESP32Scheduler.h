#pragma once
#include "AbstractTask.h"

#ifndef ESP32SCHEDULER_MAX_TASKS
  #define ESP32SCHEDULER_MAX_TASKS 10
#endif

class ESP32Scheduler {
public:
  void start(AbstractTask* task) {
    if (nextTaskId >= ESP32SCHEDULER_MAX_TASKS) {
      return;
    }

    tasks[nextTaskId++] = task;
  }

  void begin() {
    for (int i = 0; i < nextTaskId; i++) {
      tasks[i]->begin();
    }
  }

protected:
  AbstractTask* tasks[ESP32SCHEDULER_MAX_TASKS];
  int nextTaskId = 0;
};

ESP32Scheduler Scheduler;
extern ESP32Scheduler Scheduler;