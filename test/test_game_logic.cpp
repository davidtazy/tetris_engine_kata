#include <catch2/catch.hpp>

#include <Tetris/Tetris.h>

using namespace tetris;

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

TEST_CASE("tetris provide a falling tetriminos  ") {
  TestableTimer timer;

  TetrisTestable tetris(timer);
  tetris.OnResume();

  tetris.Current();
}

TEST_CASE("tetris provide next falling tetriminos ") {
  TestableTimer timer;

  TetrisTestable tetris(timer);
  tetris.OnResume();

  tetris.Next();
}
