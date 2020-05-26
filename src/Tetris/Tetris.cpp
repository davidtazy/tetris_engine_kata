#include "Tetris.h"
#include <algorithm>

namespace tetris {

Tetris::Tetris(ITimer& timer, int seed)
    : timer(timer), generator(seed), left_wall(height), right_wall(height), floor(width + 1) {
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
  if (next.IsNull()) {
    next = generator.Create();
  }
  current = next;
  next = generator.Create();
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

}  // namespace tetris