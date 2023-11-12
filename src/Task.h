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
    if ( (ms == 0 || ms < portTICK_PERIOD_MS) ) {
      yield();
      return;
    }

    vTaskDelay(ms / portTICK_PERIOD_MS);
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
    return 1;
  }

  void static xLoopWrapper(void *pvParameters) {
    static_cast<Task*>(pvParameters)->loopWrapper();
  }

  void begin() override {
    if ( setupDone || !enabled ) {
      return;
    }

    setup();
    xTaskCreatePinnedToCore(
      this->xLoopWrapper,
      getTaskName(),
      getTaskStackSize(),
      this,
      getTaskPriority(),
      &tHandle,
      getTaskCore() > (ESP.getChipCores() - 1) ? (ESP.getChipCores() - 1) : getTaskCore()
    );

    setupDone = true;
  }

  void loopWrapper() override {
    while (true) {
      loop();
      delay(interval);
    }
  }

private:
  TaskHandle_t tHandle;
};
