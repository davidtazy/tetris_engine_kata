#include <catch2/catch.hpp>

#include <Tetris/KeyboardInput.h>
#include <Tetris/NintendoClassicScore.h>
#include <Tetris/Tetris.h>

#include "Testables.h"

using namespace tetris;

TEST_CASE("can receive user events from external") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  REQUIRE(game.input_event == 0);
  auto external_lib_inputs = [](InputListener& listener) {
    listener.OnLeft();
    listener.OnRight();
    listener.OnRotate();
    listener.OnFastDown();
    listener.OnPause();
    listener.OnResume();
  };

  external_lib_inputs(game);

  REQUIRE(game.input_event == 111111);
}

TEST_CASE("can receive timer events from external ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  REQUIRE(game.timer_event == 0);
  timer.Step();
  REQUIRE(game.timer_event == 1);
}

TEST_CASE("timing requirements") {
  SECTION("tetris is suspended on Tetris instanciation") {
    TestableTimer timer;
    UserInput user_input;
    DummyScore score;
    TetriminosGenerator gen(std::random_device{}());
    TetrisTestable game(user_input, timer, score, gen, 1);

    REQUIRE(timer.IsStarted() == false);

    SECTION(" timer is started OnResume request") {
      game.OnResume();
      REQUIRE(timer.IsStarted() == true);

      SECTION(" timer is stopped  OnPause request") {
        game.OnPause();
        REQUIRE(timer.IsStarted() == false);
      }
    }
  }
}

TEST_CASE("when game begin, current and next block are available  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TestableGenerator gen;
  using t = Tetriminos::eType;
  gen.buf = std::list<Tetriminos>{Tetriminos{t::I}, Tetriminos{t::O}};

  TetrisTestable game(user_input, timer, score, gen, 1);

  REQUIRE(game.Current().Type() == t::I);
  REQUIRE(game.Next().Type() == t::O);

  SECTION("and current block is at the top /center of the game") {
    REQUIRE(game.Current().Position() == game.StartPosition());
    REQUIRE(game.Current().Position().y == 0);
    REQUIRE(game.Current().Position().x == game.Width() / 2);
  }
}

TEST_CASE("when game begin, walls and floor are available  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  REQUIRE(game.RightWall().size() == game.Height());
  REQUIRE(game.LeftWall().size() == game.Height());
  REQUIRE(game.Floor().size() == game.Width() + 2);
}

TEST_CASE("can access N next tetriminos during game") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TestableGenerator gen;
  using t = Tetriminos::eType;
  gen.buf = std::list<Tetriminos>{
      Tetriminos(t::I),
      Tetriminos(t::J),
      Tetriminos(t::L),
      Tetriminos(t::O),
  };

  int nb_next_tetri = 3;
  TetrisTestable game(user_input, timer, score, gen, nb_next_tetri);

  REQUIRE(game.Next().Type() == t::J);
  REQUIRE(game.Next(1).Type() == t::L);
  REQUIRE(game.Next(2).Type() == t::O);

  REQUIRE_THROWS_AS(game.Next(3), std::runtime_error);
}

TEST_CASE("whan game start, current block can rotate freely  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  for (int i = 0; i < 4; i++) {
    game.OnRotate();
  }
  REQUIRE(game.History().at(0) == eAction::TryRotate);
  REQUIRE(game.History().at(1) == eAction::Rotate);
  REQUIRE(game.History().back() == eAction::Rotate);
}

TEST_CASE("when game start, current block can move left until left wall  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);
  game.OnResume();

  for (int i = 0; i < game.Width(); i++) {
    game.OnLeft();
  }

  REQUIRE(game.History().at(0) == eAction::TryLeft);
  REQUIRE(game.History().at(1) == eAction::Left);
  REQUIRE(game.LastAction() == eAction::CollisionWall);

  // most left block is at position 0
  auto blocks = game.Current().BlocksAbsolutePosition();
  auto it =
      std::min_element(blocks.begin(), blocks.end(),
                       [](const Pos& first, const Pos& smallest) { return first.x < smallest.x; });

  REQUIRE(it->x == 0);
}

TEST_CASE("when game start, current block can move right until right wall  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);
  game.OnResume();

  for (int i = 0; i < game.Width(); i++) {
    game.OnRight();
  }
  REQUIRE(game.History().at(0) == eAction::TryRight);
  REQUIRE(game.History().at(1) == eAction::Right);
  REQUIRE(game.LastAction() == eAction::CollisionWall);

  // most right block is at position widrg -1
  auto blocks = game.Current().BlocksAbsolutePosition();
  auto it =
      std::max_element(blocks.begin(), blocks.end(),
                       [](const Pos& largest, const Pos& first) { return first.x > largest.x; });
  REQUIRE(it->x == game.Width() - 1);
}

TEST_CASE("during game, current block can move left until stale blocks  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  game.AddStaleBlocks({
      Pos{0, 0},
      Pos{0, 1},
      Pos{0, 2},
  });

  game.OnResume();

  for (int i = 0; i < game.Width(); i++) {
    game.OnLeft();
  }
  REQUIRE(game.History().at(0) == eAction::TryLeft);
  REQUIRE(game.History().at(1) == eAction::Left);
  REQUIRE(game.LastAction() == eAction::CollisionStale);
}

TEST_CASE("during game, current block can move right until stale blocks  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  int w = game.Width();

  game.AddStaleBlocks({
      Pos{w, 0},
      Pos{w, 1},
      Pos{w, 2},
  });

  game.OnResume();

  for (int i = 0; i < game.Width(); i++) {
    game.OnRight();
  }
  REQUIRE(game.History().front() == eAction::TryRight);
  REQUIRE(game.History().at(1) == eAction::Right);
  REQUIRE(game.LastAction() == eAction::CollisionStale);
}

TEST_CASE("during game, current block can be soft drop  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TestableGenerator gen;

  gen.buf =
      std::list<Tetriminos>{Tetriminos{Tetriminos::eType::I}, Tetriminos{Tetriminos::eType::I},
                            Tetriminos{Tetriminos::eType::I}};

  TetrisTestable game(user_input, timer, score, gen, 1);

  for (int i = 0; i < game.Height() - 1; i++) {
    game.OnFastDown();
    INFO("step " << i << "/" << game.Height() - 2);
    REQUIRE(game.LastAction() == eAction::Down);
  }

  SECTION("dont move anymore when touch floor") {
    game.OnFastDown();
    REQUIRE(game.LastAction() == eAction::Land);
  }
}

TEST_CASE("during game, current block can not rotate if collide  ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  auto w = game.Width() / 2;

  game.AddStaleBlocks({
      Pos{w - 1, 2},
      Pos{w, 2},
      Pos{w + 1, 2},
  });

  game.OnResume();

  game.SetCurrent(Tetriminos{Tetriminos::eType::I});

  game.OnRotate();
  REQUIRE(game.History() == ActionHistory{eAction::TryRotate, eAction::CollisionStale});
}

TEST_CASE("On timer event, tetriminos move down ") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  game.SetCurrent(Tetriminos{Tetriminos::eType::I});
  for (int i = 0; i < game.Height() - 1; i++) {
    timer.Step();
    INFO("step " << i << "/" << game.Height() - 2);
    REQUIRE(game.LastAction() == eAction::Down);
  }

  SECTION("dont move anymore when touch floor") {
    timer.Step();
    REQUIRE(game.LastAction() == eAction::Land);
  }
}

TEST_CASE("when landing") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  timer.Step();
  REQUIRE(game.StaleBlocks().empty());

  game.Land();
  SECTION("current tetriminos morph in stale blocks ") {
    REQUIRE(game.StaleBlocks().size() == 4);

    SECTION("next tetriminos become current one at start position") {
      REQUIRE(game.Current().Position() == Pos{game.Width() / 2, 0});
    }
  }
}

TEST_CASE("game is over") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(-793007151);
  TetrisTestable game(user_input, timer, score, gen, 1);

  timer.Step();
  REQUIRE(game.StaleBlocks().empty());

  // force first block to "land" in start position
  game.Land();

  timer.Step();
  REQUIRE(game.LastAction() == eAction::GameOver);
  REQUIRE(game.IsOver());
}
//#include <iostream>
TEST_CASE("can find completed lines") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  SECTION("stale blocks is empty") { REQUIRE(game.FindCompletedLines().empty()); }
  SECTION("no line completed") {
    game.AddStaleBlocks({Pos{5, 5}, Pos{6, 6}});

    REQUIRE(game.FindCompletedLines().empty());
  }
  SECTION("one line completed") {
    CreateCompletedLine(game, 10);
    REQUIRE(game.FindCompletedLines() == std::vector<int>{10});
  }

  SECTION("2 lines completed") {
    CreateCompletedLine(game, 10);
    CreateCompletedLine(game, 12);
    REQUIRE(game.FindCompletedLines() == std::vector<int>{10, 12});
  }

  SECTION("3 lines completed") {
    CreateCompletedLine(game, 10);
    CreateCompletedLine(game, 12);
    CreateCompletedLine(game, 13);
    REQUIRE(game.FindCompletedLines() == std::vector<int>{10, 12, 13});
  }

  SECTION("4 lines completed") {
    CreateCompletedLine(game, 10);
    CreateCompletedLine(game, 11);
    CreateCompletedLine(game, 12);
    CreateCompletedLine(game, 13);
    REQUIRE(game.FindCompletedLines() == std::vector<int>{10, 11, 12, 13});
  }
}

TEST_CASE(" can remove all blocks in one line") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  CreateCompletedLine(game, 10);

  int nb_block_to_remove = game.StaleBlocks().size();
  game.RemoveAllBlocksInLine(9);
  REQUIRE(game.StaleBlocks().size() == nb_block_to_remove);

  game.RemoveAllBlocksInLine(10);
  REQUIRE(game.StaleBlocks().empty());
}

TEST_CASE(" can apply gravity and move down all block above line") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  CreateCompletedLine(game, 7);
  CreateCompletedLine(game, 8);
  CreateCompletedLine(game, 10);

  game.ApplyGravity(9);

  REQUIRE(game.FindCompletedLines() == std::vector<int>{8, 9, 10});
}

TEST_CASE(" can apply gravity when several line were cleared") {
  TestableTimer timer;
  UserInput user_input;
  DummyScore score;
  TetriminosGenerator gen(std::random_device{}());
  TetrisTestable game(user_input, timer, score, gen, 1);

  // game.AddStaleBlocks({{5,5},{7,7},{9,9}})
  CreateCompletedLine(game, 7);
  CreateCompletedLine(game, 9);
  CreateCompletedLine(game, 11);

  game.ApplyGravity(std::vector<int>{10, 8});

  REQUIRE(game.FindCompletedLines() == std::vector<int>{9, 10, 11});
}

TEST_CASE("minimal end to end game ") {
  TestableTimer timer;

  using namespace std::literals::string_literals;

  auto user_input = KeyBoardInputsBuilder{}
                        .AssignLeft("left"s)
                        .AssignRight("right"s)
                        .AssignRotate("rotate"s)
                        .AssignMoveDown("down"s)
                        .AssignPause("pause"s)
                        .AssignResume("resume"s)
                        .Build();

  NintendoClassicScore score;
  TetriminosGenerator gen(std::random_device{}());
  Tetris game(user_input, timer, score, gen, 1);
  user_input.OnKeyPressed("resume"s);

  int pingpong{};
  while (!game.IsOver()) {
    if (pingpong % 2 == 0)
      user_input.OnKeyPressed("left"s);
    else
      user_input.OnKeyPressed("right"s);
    timer.Step();
    pingpong++;
  }

  auto h = game.History();
  REQUIRE(std::count(h.begin(), h.end(), eAction::Land) > 8);

  REQUIRE(game.StaleBlocks().size() > 8 * 4);

  REQUIRE(score.Score() > 8);

  // std::cout << dump(game) << std::endl;
}
