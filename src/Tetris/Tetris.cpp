#include "Tetris.h"
#include <algorithm>
#include <iostream>
namespace tetris {

Tetris::Tetris(ITimer& timer, IScore& score_p, ITetriminosGenerator& gen, int buffer_depth)
    : timer(timer),
      score(score_p),
      generator(gen, buffer_depth),
      left_wall(height),
      right_wall(height),
      floor(width + 1) {
  timer.Register(this);
  LoadNext();

  // generate walls
  for (int i = 0; i < right_wall.size(); i++) {
    right_wall[i].y = left_wall[i].y = i;
    right_wall[i].x = width;
  }

  // generate floor
  for (int i = 0; i < floor.size(); i++) {
    floor[i].y = height;
    floor[i].x = i;
  }
}

void Tetris::LoadNext() {
  SetCurrent(generator.Take());
}

void tetris::Tetris::SetCurrent(const Tetriminos& t) {
  current = t;
  current.SetX(width / 2);  // initial position
}

void Tetris::OnRotate() {
  auto c = current;
  actions.push_back(eAction::TryRotate);
  c.Rotate();

  if (CollideWithStaleBlocks(c)) {
    actions.push_back(eAction::CollisionStale);
    return;
  }

  if (CollideWithLeftWall(c) || CollideWithRightWall(c)) {
    actions.push_back(eAction::CollisionWall);
    return;
  }

  if (CollideWithFloor(c)) {
    actions.push_back(eAction::CollisionFloor);
    return;
  }
  current = c;
  actions.push_back(eAction::Rotate);
}

void Tetris::OnLeft() {
  auto c = current;
  actions.push_back(eAction::TryLeft);
  c.MoveLeft();

  if (CollideWithStaleBlocks(c)) {
    actions.push_back(eAction::CollisionStale);
    return;
  }

  if (CollideWithLeftWall(c)) {
    actions.push_back(eAction::CollisionWall);
    return;
  }
  current = c;
  actions.push_back(eAction::Left);
}
void Tetris::OnRight() {
  auto c = current;
  actions.push_back(eAction::TryRight);
  c.MoveRight();

  if (CollideWithStaleBlocks(c)) {
    actions.push_back(eAction::CollisionStale);
    return;
  }

  if (CollideWithRightWall(c)) {
    actions.push_back(eAction::CollisionWall);
    return;
  }
  current = c;
  actions.push_back(eAction::Right);
}

void Tetris::OnTimerEvent(const ITimer& timer) {
  if (IsOver()) {
    actions.push_back(eAction::GameOver);
    return;
  }

  auto next_pos = current;
  next_pos.MoveDown();

  if (CollideWithStaleBlocks(next_pos) || CollideWithFloor(next_pos)) {
    Land();
    if (auto completed_lines = FindCompletedLines(); completed_lines.size()) {
      for (auto line : completed_lines) {
        RemoveAllBlocksInLine(line);
      }
      for (auto line : completed_lines) {
        ApplyGravity(line);
      }
    }

    return;
  }

  actions.push_back(eAction::Down);
  current = next_pos;
}

void Tetris::Land() {
  actions.push_back(eAction::Land);

  auto blocks = MorphToBlocks(current);
  for (auto& block : blocks)
    AddStaleBlock(block);

  LoadNext();
}

bool Tetris::CollideWithLeftWall(const Tetriminos& t) const {
  return Collision(left_wall, t.BlocksAbsolutePosition());
}
bool Tetris::CollideWithRightWall(const Tetriminos& t) const {
  return Collision(right_wall, t.BlocksAbsolutePosition());
}

bool Tetris::CollideWithFloor(const Tetriminos& t) const {
  return Collision(floor, t.BlocksAbsolutePosition());
}

bool Tetris::CollideWithStaleBlocks(const Tetriminos& t) const {
  std::vector<Pos> blocks_pos(stale_blocks.size());
  std::transform(stale_blocks.begin(), stale_blocks.end(), blocks_pos.begin(),
                 [](const Block& bl) { return bl.pos; });

  return Collision(blocks_pos, t.BlocksAbsolutePosition());
}

bool tetris::Tetris::IsOver() const {
  return current.Position() == StartPosition() && CollideWithStaleBlocks(current);
}

Blocks MorphToBlocks(const Tetriminos& t) {
  auto abs_pos_t = t.BlocksAbsolutePosition();

  Blocks blocks(abs_pos_t.size());
  auto color = t.ColorHint();

  std::transform(abs_pos_t.begin(), abs_pos_t.end(), blocks.begin(), [color](const Pos& abs_pos) {
    return Block{abs_pos, color};
  });

  return blocks;
}
#include <numeric>
std::vector<int> Tetris::FindCompletedLines() const {
  std::vector<int> counters =
      std::accumulate(stale_blocks.begin(), stale_blocks.end(), std::vector<int>(height),
                      [](std::vector<int> counter, const Block& block) {
                        if (block.pos.y >= 0)
                          counter[block.pos.y]++;
                        return counter;
                      });

  std::vector<int> ret;
  for (int i = 0; i < counters.size(); i++) {
    if (counters.at(i) == width) {
      ret.push_back(i);
    }
  }

  return ret;
}

void Tetris::RemoveAllBlocksInLine(int line) {
  auto it = std::remove_if(stale_blocks.begin(), stale_blocks.end(),
                           [line](const Block& block) { return block.pos.y == line; });
  stale_blocks.erase(it, stale_blocks.end());
}

void Tetris::ApplyGravity(int line) {
  for (Block& block : stale_blocks) {
    auto& y = block.pos.y;
    if (y < line)
      y++;
  }
}

}  // namespace tetris
