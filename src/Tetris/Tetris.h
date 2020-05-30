#pragma once
#include <chrono>
#include "Tetris/IScore.h"
#include "Tetris/ITimer.h"
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

struct Block {
  Pos pos;
  Tetriminos::eColor color;
};
using Blocks = std::vector<Block>;

enum class eAction {
  NoAction,
  TryLeft,
  TryRight,
  TryRotate,
  FastFastDown,
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
  IScore& score;
  TetriminosFactory generator;
  Tetriminos current;
  Blocks stale_blocks;
  ActionHistory actions;
  int width{10};
  int height{25};
  std::vector<Pos> left_wall;
  std::vector<Pos> right_wall;
  std::vector<Pos> floor;
  //

 public:
  // int seed =
  explicit Tetris(ITimer& timer, IScore& score_p, ITetriminosGenerator& gen, int buffer_depth);

  int Width() const { return width; }
  int Height() const { return height; }
  Pos StartPosition() const { return Pos{width / 2, 0}; }

  ///  events
  void OnLeft() override;
  void OnRight() override;
  void OnRotate() override;
  void OnFastDown() override;
  void OnPause() override { timer.Stop(); }
  void OnResume() override {
    if (!timer.IsStarted())
      timer.Start(std::chrono::seconds{1});
  }
  void Down();
  void OnTimerEvent(const ITimer& timer) override;

  // blocks

  Tetriminos Current() const { return current; }
  Tetriminos Next(int offset = 0) const { return generator.Next(offset); }
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

  std::vector<int> FindCompletedLines() const;
  Blocks MorphToBlocks(const Tetriminos& t) const;

 protected:
  void LoadNext();
  bool CollideWithLeftWall(const Tetriminos& t) const;
  bool CollideWithRightWall(const Tetriminos& t) const;
  bool CollideWithStaleBlocks(const Tetriminos& t) const;
  bool CollideWithFloor(const Tetriminos& t) const;
  void Land();

  void RemoveAllBlocksInLine(int line);
  void ApplyGravity(std::vector<int> line);
  void ApplyGravity(int line);
  void AddStaleBlock(const Block& block) { stale_blocks.push_back(block); }

  void SetCurrent(const Tetriminos& t);

  void ThrowIfOffGridBlock(const Pos& pos) const;
};

}  // namespace tetris
