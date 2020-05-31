#pragma once
#include <chrono>
struct ITimer;
struct TimerListener {
  virtual void OnTimerEvent(const ITimer& timer) = 0;
};

struct ITimer {
  virtual void Start(const std::chrono::milliseconds& period) = 0;
  virtual void Stop() = 0;
  bool IsStarted() const { return started; }
  void Register(TimerListener* listener_p) { listener = listener_p; };

 protected:
  void Step() {
    if (listener)
      listener->OnTimerEvent(*this);
  }

  bool started{false};

 private:
  TimerListener* listener{};
};