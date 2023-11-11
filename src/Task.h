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
    vTaskDelay(ms / portTICK_PERIOD_MS);
  }

protected:
  const char* taskName = "";
  const int taskCore = 1;
  const uint32_t taskStackSize = 10000;

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
      taskName,
      taskStackSize,
      this,
      1,
      &tHandle,
      taskCore > (ESP.getChipCores() - 1) ? (ESP.getChipCores() - 1) : taskCore
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
