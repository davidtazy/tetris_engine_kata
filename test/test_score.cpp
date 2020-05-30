#include <Tetris/IScore.h>
#include <Tetris/Tetris.h>

std::ostream& operator<<(std::ostream& out, const std::chrono::milliseconds& delay) {
  return out << delay.count() << "ms";
}

#include <catch2/catch.hpp>

#include "Testables.h"

struct MockScore : tetris::IScore {
  int new_tetri{};
  int comp_line_call{};
  int comp_line_arg{};
  int perfect_clear{};
  int soft_drop{};
  bool change_level{false};
  void OnNewTetriminos() override { new_tetri++; };
  bool OnCompletedLine(int nb_line) override {
    comp_line_arg = nb_line;
    comp_line_call++;
    return change_level;
  };
  void OnPerfectClear() override { perfect_clear++; };
  void OnSoftDrop() override { soft_drop++; };
  int Score() const override { return 0; };
  int Level() const override { return 1; };
  int CompletedLines() const override { return 2; };
  std::chrono::milliseconds DropPeriod(int level) const override {
    return std::chrono::seconds{1};
  };
};

using namespace tetris;

TEST_CASE("new tetrimino call on each new tetrimino") {
  TestableTimer timer;
  UserInput user_input;
  MockScore score;
  TetriminosGenerator gen(std::random_device{}());

  // on instanciation
  TetrisTestable game(user_input, timer, score, gen, 1);
  REQUIRE(score.new_tetri == 1);

  game.Land();
  REQUIRE(score.new_tetri == 2);
}

TEST_CASE("completed line call ") {
  TestableTimer timer;
  UserInput user_input;
  MockScore score;
  TestableGenerator gen;
  gen.buf = std::list<Tetriminos>{Tetriminos{"I"}, Tetriminos{"I"}, Tetriminos{"I"}};

  TetrisTestable game(user_input, timer, score, gen, 1);
  game.OnResume();

  timer.Step();
  REQUIRE(score.comp_line_call == 0);  // no new line

  CreateCompletedLine(game, 2);  // force land
  CreateCompletedLine(game, 3);

  timer.Step();

  REQUIRE(score.comp_line_call == 1);
  REQUIRE(score.comp_line_arg == 2);
}

TEST_CASE("perfect clear call ") {
  TestableTimer timer;
  UserInput user_input;
  MockScore score;
  TestableGenerator gen;
  gen.buf = std::list<Tetriminos>{Tetriminos{"I"}, Tetriminos{"I"}, Tetriminos{"I"}};

  TetrisTestable game(user_input, timer, score, gen, 1);
  game.OnResume();

  timer.Step();
  REQUIRE(score.perfect_clear == 0);  // no perfect clear

  CreateLine(game, 1, "#xxxxx....x#");  // complete line
  CreateLine(game, 2, "#xxxxxxxxxx#");  // force land

  timer.Step();
  INFO(dump(game));
  REQUIRE(game.StaleBlocks().size() == 0);

  REQUIRE(score.perfect_clear == 1);
}

TEST_CASE("soft drop call") {
  TestableTimer timer;
  UserInput user_input;
  MockScore score;
  TetriminosGenerator gen(std::random_device{}());

  // on instanciation
  TetrisTestable game(user_input, timer, score, gen, 1);

  game.OnFastDown();

  REQUIRE(score.soft_drop == 1);
}

TEST_CASE("restart timer on level change") {
  TestableTimer timer;
  UserInput user_input;
  MockScore score;
  TestableGenerator gen;
  gen.buf = std::list<Tetriminos>{Tetriminos{"I"}, Tetriminos{"I"}, Tetriminos{"I"}};

  TetrisTestable game(user_input, timer, score, gen, 1);
  game.OnResume();
  REQUIRE(timer.start_call == 1);

  timer.Step();

  CreateCompletedLine(game, 2);  // force land
  CreateCompletedLine(game, 3);

  score.change_level = true;

  timer.Step();

  REQUIRE(timer.start_call == 2);
}

/////////////////////////////////////////////////
///   Nintendo Gaming Scoring ///////////////////
#include <Tetris/NintendoClassicScore.h>
TEST_CASE(" Nintendo gaming scoring") {
  NintendoClassicScore score;
  REQUIRE(score.Score() == 0);
  REQUIRE(score.CompletedLines() == 0);

  SECTION(" default level is 1") { REQUIRE(score.Level() == 1); }

  SECTION(" score increment 1 on soft drop") {
    score.OnSoftDrop();
    REQUIRE(score.Score() == 1);
  }
  SECTION(" score increment 1 on new tetriminos") {
    score.OnNewTetriminos();
    REQUIRE(score.Score() == 1);
  }

  SECTION(" score on completed line") {
    auto [nb_line, expected_score] =
        GENERATE(table<int, int>({{1, 40}, {2, 100}, {3, 300}, {4, 1200}}));

    score.OnCompletedLine(nb_line);
    INFO("nb_line: " << nb_line << " expected score: " << expected_score);
    REQUIRE(score.Score() == expected_score);
  }

  SECTION(" drop period calculation") {
    using namespace std::literals::chrono_literals;
    auto [level, expected_delay] =
        GENERATE(table<int, std::chrono::milliseconds>({{1, 999ms}, {2, 793ms}, {3, 617ms}}));
    INFO("level: " << level);
    REQUIRE(score.DropPeriod(level) == expected_delay);
  }

  SECTION("no gain on perfect clear") {
    score.OnPerfectClear();
    REQUIRE(score.Score() == 0);
  }
}