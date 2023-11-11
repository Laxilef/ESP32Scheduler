#pragma once
#include <Arduino.h>
#include "Task.h"

class LeanTask : public Task {
public:
  LeanTask(bool enabled = true, unsigned long interval = 0) : Task(enabled, interval) {}
};