#pragma once
#include <Arduino.h>

class AbstractTask {
public:
  AbstractTask(bool enabled = true, unsigned long interval = 0) :
    enabled(enabled),
    interval(interval)
  {
  }

  virtual bool isEnabled() {
    return enabled;
  }

  virtual void enable() {
    enabled = true;
  }

  virtual void disable() {
    enabled = false;
  }

  void setInterval(unsigned long val) {
    interval = val;
  }

  unsigned long getInterval() {
    return interval;
  }

  virtual void yield() = 0;
  virtual void delay(unsigned long ms) = 0;


protected:
  friend class ESP32Scheduler;
  bool setupDone = false;
  bool enabled = true;
  unsigned long interval = 0;

  virtual void begin() {
    if (setupDone) {
      return;
    }

    setup();
    setupDone = true;
  }

  virtual void setup() {}
  virtual void loop() {}
  virtual void loopWrapper() = 0;
};
