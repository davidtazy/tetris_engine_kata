#pragma once
#include <chrono>
#include "Tetris/Tetriminos.h"
namespace tetris {

enum class UserEvent {
  Left,
  Right,
  Rotate,
  FastDown,
  Pause,
  Resume,
};

struct InputListener {
  virtual void OnLeft() = 0;
  virtual void OnRight() = 0;
  virtual void OnRotate() = 0;
  virtual void OnFastDown() = 0;
  virtual void OnPause() = 0;
  virtual void OnResume() = 0;
};

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

struct TestableTimer : public ITimer {
  void Start(const std::chrono::milliseconds& period) { started = true; }
  void Stop() { started = false; }
  using ITimer::Step;
};

class Tetris : public InputListener, public TimerListener {
  ITimer& timer;

 public:
  explicit Tetris(ITimer& timer) : timer(timer) { timer.Register(this); }

  ///  events
  void OnLeft() override {}
  void OnRight() override {}
  void OnRotate() override {}
  void OnFastDown() override {}
  void OnPause() override { timer.Stop(); }
  void OnResume() override {
    if (!timer.IsStarted())
      timer.Start(std::chrono::seconds{1});
  }
  void OnTimerEvent(const ITimer& timer) override {}

  // blocks

  Tetriminos Current() const { return {}; }
  Tetriminos Next() const { return {}; }

 protected:
  void LoadNext();
};

}  // namespace tetris
