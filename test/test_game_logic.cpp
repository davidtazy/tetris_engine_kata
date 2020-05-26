#include <catch2/catch.hpp>

#include <Tetris/Tetris.h>

using namespace tetris;

struct TestableTimer : public ITimer {
  void Start(const std::chrono::milliseconds& period) { started = true; }
  void Stop() { started = false; }
  using ITimer::Step;
};

struct TetrisTestable : public Tetris {
  using Tetris::Tetris;
  void OnLeft() override {
    Tetris::OnLeft();
    input_event++;
  }
  void OnRight() override {
    Tetris::OnRight();
    input_event += 10;
  }
  void OnRotate() override {
    Tetris::OnRotate();
    input_event += 100;
  }
  void OnFastDown() override {
    Tetris::OnFastDown();
    input_event += 1000;
  }
  void OnPause() override {
    Tetris::OnPause();
    input_event += 10000;
  }
  void OnResume() override {
    Tetris::OnResume();
    input_event += 100000;
  }
  void OnTimerEvent(const ITimer& timer) override {
    Tetris::OnTimerEvent(timer);
    timer_event++;
  }

  using Tetris::AddStaleBlock;

  int input_event{};
  int timer_event{};
};

TEST_CASE("can receive user events from external lib") {
  TestableTimer timer;
  TetrisTestable tetris(timer);

  REQUIRE(tetris.input_event == 0);
  auto external_lib_inputs = [](InputListener& listener) {
    listener.OnLeft();
    listener.OnRight();
    listener.OnRotate();
    listener.OnFastDown();
    listener.OnPause();
    listener.OnResume();
  };

  external_lib_inputs(tetris);

  REQUIRE(tetris.input_event == 111111);
}

TEST_CASE("can receive timer events from external lib") {
  TestableTimer timer;

  TetrisTestable tetris(timer);
  REQUIRE(tetris.timer_event == 0);
  timer.Step();
  REQUIRE(tetris.timer_event == 1);
}

TEST_CASE("timing requirements") {
  SECTION("tetris is suspended on Tetris instanciation") {
    TestableTimer timer;

    TetrisTestable tetris(timer);

    REQUIRE(timer.IsStarted() == false);

    SECTION(" timer is started OnResume request") {
      tetris.OnResume();
      REQUIRE(timer.IsStarted() == true);

      SECTION(" timer is stopped  OnPause request") {
        tetris.OnPause();
        REQUIRE(timer.IsStarted() == false);
      }
    }
  }
}

TEST_CASE("when game begin, current and next block are available  ") {
  TestableTimer timer;

  Tetris tetris(timer);

  REQUIRE_FALSE(tetris.Current().IsNull());
  REQUIRE_FALSE(tetris.Next().IsNull());

  SECTION("and current block is at the top /center of the game") {
    REQUIRE(tetris.Current().Position().y == 0);
    REQUIRE(tetris.Current().Position().x == tetris.Width() / 2);
  }
}

TEST_CASE("when game begin, walls and floor are available  ") {
  TestableTimer timer;

  Tetris tetris(timer);

  REQUIRE(tetris.RightWall().size() == tetris.Height());
  REQUIRE(tetris.LeftWall().size() == tetris.Height());
  REQUIRE(tetris.Floor().size() == tetris.Width() + 1);
}

TEST_CASE("whan game start, current block can rotate freely  ") {
  TestableTimer timer;
  TetrisTestable game(timer);

  for (int i = 0; i < 4; i++) {
    game.OnRotate();
  }
  REQUIRE(game.History().at(0) == eAction::TryRotate);
  REQUIRE(game.History().at(1) == eAction::Rotate);
  REQUIRE(game.History().back() == eAction::Rotate);
}

TEST_CASE("when game start, current block can move left until left wall  ") {
  TestableTimer timer;

  Tetris game(timer);
  game.OnResume();

  for (int i = 0; i < game.Width(); i++) {
    game.OnLeft();
  }
  REQUIRE(game.History().at(0) == eAction::TryLeft);
  REQUIRE(game.History().at(1) == eAction::Left);
  REQUIRE(game.LastAction() == eAction::CollisionWall);
}

TEST_CASE("when game start, current block can move right until right wall  ") {
  TestableTimer timer;

  Tetris game(timer);
  game.OnResume();

  for (int i = 0; i < game.Width(); i++) {
    game.OnRight();
  }
  REQUIRE(game.History().at(0) == eAction::TryRight);
  REQUIRE(game.History().at(1) == eAction::Right);
  REQUIRE(game.LastAction() == eAction::CollisionWall);
}

TEST_CASE("during game, current block can move left until stale blocks  ") {
  TestableTimer timer;

  TetrisTestable game(timer);

  game.AddStaleBlock(Block{Pos{0, 0}, Tetriminos::eColor::Blue});
  game.AddStaleBlock(Block{Pos{0, 1}, Tetriminos::eColor::Blue});
  game.AddStaleBlock(Block{Pos{0, 2}, Tetriminos::eColor::Blue});

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

  TetrisTestable game(timer);

  int w = game.Width();

  game.AddStaleBlock(Block{Pos{w, 0}, Tetriminos::eColor::Blue});
  game.AddStaleBlock(Block{Pos{w, 1}, Tetriminos::eColor::Blue});
  game.AddStaleBlock(Block{Pos{w, 2}, Tetriminos::eColor::Blue});

  game.OnResume();

  for (int i = 0; i < game.Width(); i++) {
    game.OnRight();
  }
  REQUIRE(game.History().front() == eAction::TryRight);
  REQUIRE(game.History().at(1) == eAction::Right);
  REQUIRE(game.LastAction() == eAction::CollisionStale);
}
