#include <Tetris/KeyboardInput.h>
#include <Tetris/NintendoClassicScore.h>
#include <Tetris/PollingTimer.h>
#include <Tetris/Tetris.h>
#include <iostream>
#include "rlutil.h"

using namespace tetris;

int cpt{};

void Draw(const Tetris& game);

int main() {
  PollingTimer timer;

  auto user_input =
      KeyBoardInputsBuilder{}
          .AssignLeft<int>(rlutil::KEY_LEFT)  // force cast because rlutil use unamed enum...
          .AssignRight<int>(rlutil::KEY_RIGHT)
          .AssignRotate<int>(rlutil::KEY_UP)
          .AssignMoveDown<int>(rlutil::KEY_DOWN)
          .AssignPause<int>(rlutil::KEY_SPACE)
          .AssignResume<int>(rlutil::KEY_ENTER)
          .Build();

  NintendoClassicScore score;
  TetriminosGenerator gen(std::random_device{}());
  Tetris game(user_input, timer, score, gen, 1);

  Draw(game);

  while (!game.IsOver()) {
    cpt++;

    if (kbhit()) {
      int k = rlutil::getkey();  // Get character

      if (user_input.IsAssignedKey(k)) {
        user_input.OnKeyPressed(k);
        Draw(game);
      }
    }

    if (timer.Poll()) {
      Draw(game);
    }
  }
}

void Draw(const Tetris& game) {
  rlutil::cls();
  rlutil::hidecursor();

  const int rl_offset = 2;

  for (auto p : game.LeftWall()) {
    gotoxy(p.x + rl_offset, p.y);
    std::cout << '#';
  }
  for (auto p : game.RightWall()) {
    gotoxy(p.x + rl_offset, p.y);
    std::cout << '#';
  }
  for (auto p : game.Floor()) {
    gotoxy(p.x + rl_offset, p.y);
    std::cout << '#';
  }
  for (auto b : game.StaleBlocks()) {
    auto p = b.pos;
    gotoxy(p.x + rl_offset, p.y);
    std::cout << 'x';
  }

  for (auto p : game.Current().BlocksAbsolutePosition()) {
    gotoxy(p.x + rl_offset, p.y);
    std::cout << '@';
  }

  // print next
  gotoxy(15, 6);
  std::cout << "next: ";
  auto next = game.Next();
  next.SetX(22);
  next.SetY(6);
  for (auto p : next.BlocksAbsolutePosition()) {
    gotoxy(p.x + rl_offset, p.y);
    std::cout << '@';
  }

  gotoxy(15, 12);
  std::cout << "score: " << game.Scoring().Score();
  gotoxy(15, 13);
  std::cout << "lines: " << game.Scoring().CompletedLines();
  gotoxy(15, 14);
  std::cout << "level: " << game.Scoring().Level();

  if (game.IsPause()) {
    gotoxy(15, 18);

    std::cout << "press <Enter> to start";
  }

  std::cout.flush();
}