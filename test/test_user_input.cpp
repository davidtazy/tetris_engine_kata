#include <catch2/catch.hpp>

#include <Tetris/IUserInput.h>
#include <Tetris/KeyboardInput.h>

using namespace tetris;

struct TestableInputListener : InputListener {
  int on_left_call{};
  int on_right_call{};
  int on_rotate_call{};
  int on_down_call{};
  int on_pause_call{};
  int on_resume_call{};
  void OnLeft() override { on_left_call++; }
  void OnRight() override { on_right_call++; }
  void OnRotate() override { on_rotate_call++; }
  void OnFastDown() override { on_down_call++; }
  void OnPause() override { on_pause_call++; }
  void OnResume() override { on_resume_call++; }
};

TEST_CASE("keyboard implementation work is minimal") {
  KeyBoardInputs input = KeyBoardInputsBuilder{}
                             .AssignLeft('s')
                             .AssignRight('d')
                             .AssignRotate('r')
                             .AssignMoveDown('w')
                             .AssignPause('p')
                             .AssignResume('x')
                             .Build();

  TestableInputListener listener;

  input.SetListener(listener);

  input.OnKeyPressed('s');
  REQUIRE(listener.on_left_call == 1);

  input.OnKeyPressed('d');
  REQUIRE(listener.on_right_call == 1);

  input.OnKeyPressed('r');
  REQUIRE(listener.on_rotate_call == 1);

  input.OnKeyPressed('p');
  REQUIRE(listener.on_pause_call == 1);

  input.OnKeyPressed('x');
  REQUIRE(listener.on_resume_call == 1);

  input.OnKeyPressed('w');
  REQUIRE(listener.on_down_call == 1);

  SECTION("throw exception if user key is not assigned") {
    REQUIRE_THROWS_AS(input.OnKeyPressed("not assigned user input"), std::runtime_error);
  }
  SECTION("cannot use the same value to assign different keys") {
    REQUIRE_THROWS_AS(KeyBoardInputsBuilder{}.AssignLeft('s').AssignRight('s').Build(),
                      std::runtime_error);
  }

  SECTION(" allow repetition key timer") {
    using namespace std::literals::chrono_literals;
    KeyBoardInputs k =
        KeyBoardInputsBuilder{}.AssignLeft('s').AssignRight('d').EnableRepeatDelay(50ms).Build();
  }
}