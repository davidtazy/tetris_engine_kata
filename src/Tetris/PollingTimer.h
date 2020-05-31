#pragma once
#include <Tetris/ITimer.h>
#include <thread>
namespace tetris {

struct PollingTimer : public ITimer {
  void Start(const std::chrono::milliseconds& period_p) {
    begin = std::chrono::steady_clock::now();
    period = period_p;
    started = true;
  }
  void Stop() override { started = false; }

  bool Poll() {
    if (!started)
      return false;
    auto now = std::chrono::steady_clock::now();
    auto delay = std::chrono::duration_cast<std::chrono::microseconds>(now - begin);

    if (delay > period) {
      begin = now;
      Step();
      return true;
    }
    return false;
  }

 private:
  std::chrono::steady_clock::time_point begin;
  std::chrono::milliseconds period;
};

struct TimerMock : TimerListener {
  virtual void OnTimerEvent(const ITimer& timer) { call++; };
  int call{};
};

}  // namespace tetris
