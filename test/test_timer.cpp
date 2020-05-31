
#include <Tetris/PollingTimer.h>
#include <catch2/catch.hpp>
using namespace tetris;
using namespace std::literals::chrono_literals;

TEST_CASE("polling timer") {
  PollingTimer timer;
  TimerMock mock;
  timer.Register(&mock);

  REQUIRE(timer.IsStarted() == false);

  timer.Start(90ms);
  REQUIRE(timer.IsStarted() == true);

  std::this_thread::sleep_for(50ms);
  REQUIRE(timer.Poll() == false);
  REQUIRE(mock.call == 0);

  std::this_thread::sleep_for(50ms);
  REQUIRE(timer.Poll() == true);

  REQUIRE(mock.call == 1);
}