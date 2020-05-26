#include <catch2/catch.hpp>

#include <Tetris/Tetriminos.h>

TEST_CASE("tetriminos as 7 types") {
  REQUIRE(static_cast<int>(tetris::Tetriminos::eType::Count) == 7);
}

TEST_CASE("tetriminos provide color hint") {
  using namespace tetris;
  using ty = tetris::Tetriminos::eType;
  using col = tetris::Tetriminos::eColor;

  auto [tetri_type, tetri_color] = GENERATE(table<Tetriminos::eType, Tetriminos::eColor>({
      {ty::I, col::Cyan},
      {ty::O, col::Yellow},
      {ty::T, col::Purple},
      {ty::L, col::Orange},
      {ty::J, col::Blue},
      {ty::Z, col::Red},
      {ty::S, col::Green},
  }));
  INFO(tetri_type << " " << tetri_color);
  REQUIRE(Tetriminos::ToColor(tetri_type) == tetri_color);
}

auto generate_tetri_type_sequence(std::vector<std::string> sequence) {
  std::vector<tetris::Tetriminos::eType> seq(sequence.size());

  std::transform(sequence.begin(), sequence.end(), seq.begin(),
                 [](const std::string& s) { return tetris::Tetriminos(s).Type(); });

  return seq;
}

TEST_CASE("block generator generate reproducible random tetriminos") {
  tetris::TetriminosGenerator gen(12345);

  auto expected_seq = generate_tetri_type_sequence({
      "S",
      "I",
      "L",
      "O",
      "Z",
      "Z",
      "Z",
      "Z",
      "I",
      "J",
      "I",
      "T",
  });

  std::vector<tetris::Tetriminos::eType> sequence(12);
  std::generate(sequence.begin(), sequence.end(), [&gen]() { return gen.Create().Type(); });

  REQUIRE(sequence == expected_seq);

  SECTION(" all type are generated") {
    std::sort(sequence.begin(), sequence.end());
    auto last = std::unique(sequence.begin(), sequence.end());
    sequence.erase(last, sequence.end());

    REQUIRE(sequence.size() == tetris::Tetriminos::BlockTypeCount());
  }
}

TEST_CASE("tetriminos has an absolute position") {
  tetris::Tetriminos t{tetris::Tetriminos::eType::T};

  auto pos = t.Position();
  REQUIRE(pos.x == 0);
  REQUIRE(pos.y == 0);
}

TEST_CASE("tetrimininos is composed of blocks") {
  tetris::Tetriminos t{tetris::Tetriminos::eType::I};

  REQUIRE(t.BlocksPosition().size() == 4);
}

TEST_CASE("tetrimininos blocks can rotate") {
  tetris::Tetriminos t{tetris::Tetriminos::eType::I};

  t.Rotate();
  auto blocks = t.BlocksPosition();
  REQUIRE(blocks.at(0) == tetris::Pos{0, 0});
  REQUIRE(blocks.at(1) == tetris::Pos{0, 1});
  REQUIRE(blocks.at(2) == tetris::Pos{0, 2});
  REQUIRE(blocks.at(3) == tetris::Pos{0, 3});

  t.Rotate();
  blocks = t.BlocksPosition();
  REQUIRE(blocks.at(1) == tetris::Pos{-1, 0});

  t.Rotate();
  blocks = t.BlocksPosition();
  REQUIRE(blocks.at(1) == tetris::Pos{0, -1});

  t.Rotate();
  blocks = t.BlocksPosition();
  REQUIRE(blocks.at(1) == tetris::Pos{1, 0});
}

TEST_CASE("tetriminos can  move down left and right") {
  tetris::Tetriminos t{tetris::Tetriminos::eType::I};

  const auto pos = t.Position();

  t.MoveDown();
  REQUIRE(t.Position().x == pos.x);
  REQUIRE(t.Position().y == pos.y + 1);

  t.MoveLeft();
  REQUIRE(t.Position().x == pos.x - 1);
  REQUIRE(t.Position().y == pos.y + 1);

  t.MoveRight();
  REQUIRE(t.Position().x == pos.x);
  REQUIRE(t.Position().y == pos.y + 1);
}

TEST_CASE("can detect collision between blocks") {
  std::vector<tetris::Pos> a{{0, 0}, {1, 0}, {2, 0}};

  std::vector<tetris::Pos> not_a{{1, 1}, {0, 1}, {0, 2}};

  std::vector<tetris::Pos> intersect_a{{-1, 0}, {1, 0}, {-2, 0}};

  REQUIRE(tetris::Collision(a, not_a) == false);
  REQUIRE(tetris::Collision(not_a, a) == false);
  REQUIRE(tetris::Collision(intersect_a, a) == true);
  REQUIRE(tetris::Collision(a, intersect_a) == true);
}