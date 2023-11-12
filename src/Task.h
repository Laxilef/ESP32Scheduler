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

  virtual int getTaskCore() {
    return 0;
  }

  virtual uint32_t getTaskStackSize() {
    return 10000;
  }

  virtual int getTaskPriority() {
    return 0;
  }

  void static xLoopWrapper(void* pvParameters) {
    static_cast<Task*>(pvParameters)->loopWrapper();
  }

  void begin() override {
    if (!enabled || tHandle != nullptr) {
      return;
    }

    xTaskCreatePinnedToCore(
      this->xLoopWrapper,
      getTaskName(),
      getTaskStackSize(),
      this,
      getTaskPriority(),
      &tHandle,
      getTaskCore() > (ESP.getChipCores() - 1) ? (ESP.getChipCores() - 1) : getTaskCore()
    );
  }

  void loopWrapper() override {
    while (true) {
      if (!setupDone) {
        setup();
        setupDone = true;
      }

      yield();
      loop();

      if (interval == 0) {
        yield();

      } else {
        delay(interval);
      }
    }
  }

private:
  TaskHandle_t tHandle = nullptr;
};
