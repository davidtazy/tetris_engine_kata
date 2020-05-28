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

struct Block {
  Pos pos;
  Tetriminos::eColor color;
};
using Blocks = std::vector<Block>;

Blocks MorphToBlocks(const Tetriminos& t);

enum class eAction {
  NoAction,
  TryLeft,
  TryRight,
  TryRotate,
  TryDown,

  Left,
  Right,
  Rotate,
  Down,

  CollisionWall,
  CollisionStale,
  CollisionFloor,

  Land,
  GameOver,
};

using ActionHistory = std::vector<eAction>;

class Tetris : public InputListener, public TimerListener {
  ITimer& timer;
  TetriminosGenerator generator;
  Tetriminos current;
  Tetriminos next;
  Blocks stale_blocks;
  ActionHistory actions;
  int width{10};
  int height{25};
  std::vector<Pos> left_wall;
  std::vector<Pos> right_wall;
  std::vector<Pos> floor;

 public:
  explicit Tetris(ITimer& timer, int seed = std::random_device{}());

  int Width() const { return width; }
  int Height() const { return height; }
  Pos StartPosition() const { return Pos{width / 2, 0}; }

  ///  events
  void OnLeft() override;
  void OnRight() override;
  void OnRotate() override;
  void OnFastDown() override {}
  void OnPause() override { timer.Stop(); }
  void OnResume() override {
    if (!timer.IsStarted())
      timer.Start(std::chrono::seconds{1});
  }
  void OnTimerEvent(const ITimer& timer) override;

  // blocks

  Tetriminos Current() const { return current; }
  Tetriminos Next() const { return next; }
  const Blocks& StaleBlocks() const { return stale_blocks; }
  const std::vector<Pos>& LeftWall() const { return left_wall; }
  const std::vector<Pos>& RightWall() const { return right_wall; }
  const std::vector<Pos>& Floor() const { return floor; }

  const ActionHistory& History() const { return actions; }
  eAction LastAction() const {
    if (actions.empty())
      return eAction::NoAction;
    return actions.back();
  }

  bool IsOver() const;

  // score related
  std::vector<int> FindCompletedLines() const;

 protected:
  void LoadNext();
  bool CollideWithLeftWall(const Tetriminos& t) const;
  bool CollideWithRightWall(const Tetriminos& t) const;
  bool CollideWithStaleBlocks(const Tetriminos& t) const;
  bool CollideWithFloor(const Tetriminos& t) const;
  void Land();

  void RemoveAllBlocksInLine(int line);
  void ApplyGravity(int line);
  void AddStaleBlock(const Block& block) { stale_blocks.push_back(block); }

  void SetCurrent(const Tetriminos& t);
};

}  // namespace tetris
