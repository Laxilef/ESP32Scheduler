#pragma once
#include <Arduino.h>
#include "AbstractTask.h"

class Task : public AbstractTask {
public:
  Task(bool enabled = true, unsigned long interval = 0) : AbstractTask(enabled, interval) {}

  void enable() override {
    AbstractTask::enable();
    begin();
    vTaskResume(tHandle);
  }

  void disable() override {
    AbstractTask::disable();
    vTaskSuspend(tHandle);
  }

  void yield() override {
    taskYIELD();
  }

  void delay(unsigned long ms) override {
    vTaskDelay((ms == 0 || ms < portTICK_PERIOD_MS) ? 1 : ms / portTICK_PERIOD_MS);
  }

protected:
  virtual const char* getTaskName() {
    return "";
  }

  virtual BaseType_t getTaskCore() {
    return tskNO_AFFINITY;
  }

  virtual uint32_t getTaskStackSize() {
    return 10000;
  }

  virtual int getTaskPriority() {
    return 1;
  }

  void static xLoopWrapper(void* pvParameters) {
    Task* task = static_cast<Task*>(pvParameters);
    while (true) {
      task->loopWrapper();
    }
  }

  void begin() override {
    if (!enabled || tHandle != nullptr) {
      return;
    }

    BaseType_t coreId;
    if (getTaskCore() == tskNO_AFFINITY || ESP.getChipCores() == 1 || getTaskCore() > (ESP.getChipCores() - 1)) {
      coreId = tskNO_AFFINITY;
      
    } else {
      coreId = getTaskCore();
    }

    xTaskCreatePinnedToCore(
      this->xLoopWrapper,
      getTaskName(),
      getTaskStackSize(),
      this,
      getTaskPriority(),
      &tHandle,
      coreId
    );
  }

  void loopWrapper() override {
    if (!setupDone) {
      setup();
      setupDone = true;
      yield();
    }

    loop();

    if (interval == 0) {
      yield();

    } else {
      delay(interval);
    }
  }

private:
  TaskHandle_t tHandle = nullptr;
};
