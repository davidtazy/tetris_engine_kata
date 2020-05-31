#pragma once
#include <array>
#include <functional>
#include <list>
#include <ostream>
#include <random>
namespace tetris {

struct Pos {
  int x{};
  int y{};

  bool operator==(const Pos& other_p) const { return x == other_p.x && y == other_p.y; }
};

std::ostream& operator<<(std::ostream& out, const Pos& pos);

bool Collision(const std::vector<Pos>& a, const std::vector<Pos>& b);

struct Tetriminos {
  enum class eType { I, O, T, L, J, Z, S, Count };
  static constexpr size_t BlockTypeCount() { return static_cast<size_t>(eType::Count); }
  using Collection = std::array<Tetriminos::eType, static_cast<size_t>(eType::Count)>;

  enum class eColor { Cyan, Yellow, Purple, Orange, Blue, Red, Green, Count };

  explicit Tetriminos(eType type_p);
  explicit Tetriminos(std::string type_p);
  Tetriminos() = default;

  eType Type() const { return type; }
  eColor ColorHint() const { return ToColor(type); }

  static constexpr Collection TypeCollection();

  static eColor ToColor(eType type_p);

  Pos Position() const { return position; }

  const std::vector<Pos>& BlocksPosition() const { return blocks; }
  std::vector<Pos> BlocksAbsolutePosition() const;

  void Rotate();
  void MoveDown();
  void MoveLeft();
  void MoveRight();
  void SetX(int x) { position.x = x; }
  void SetY(int y) { position.y = y; }

 private:
  eType type;
  Pos position;
  std::vector<Pos> blocks;
};
std::ostream& operator<<(std::ostream& out, const Tetriminos::eType& type);
std::ostream& operator<<(std::ostream& out, const Tetriminos::eColor& color);

//! some flavour of tetris has strategy to deliver Tetriminos
struct ITetriminosGenerator {
  virtual Tetriminos Create() = 0;
};

class TetriminosFactory {
  ITetriminosGenerator& generator;
  std::list<Tetriminos> buffer;

 public:
  explicit TetriminosFactory(ITetriminosGenerator& generator_p, int buffer_depth)
      : generator(generator_p), buffer(buffer_depth) {
    std::generate(buffer.begin(), buffer.end(),
                  std::bind(&ITetriminosGenerator::Create, std::ref(generator)));
  }

  Tetriminos Take() {
    auto t = buffer.front();
    buffer.pop_front();

    buffer.push_back(generator.Create());

    return t;
  }

  Tetriminos Next(int offset = 0) const {
    if (offset < 0 || offset >= buffer.size()) {
      throw std::runtime_error("try to access not allowed tetriminos");
    }

    return *std::next(buffer.begin(), offset);
  }
};

struct TetriminosGenerator : ITetriminosGenerator {
  explicit TetriminosGenerator(int seed) { gen.seed(seed); };
  Tetriminos Create() override;

 private:
  std::mt19937 gen;
};

}  // namespace tetris
