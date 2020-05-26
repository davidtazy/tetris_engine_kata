#pragma once
#include <array>
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

  std::vector<Pos> BlocksPosition() { return blocks; }

  void Rotate();

 private:
  eType type;
  Pos position;
  std::vector<Pos> blocks;
};
std::ostream& operator<<(std::ostream& out, const Tetriminos::eType& type);
std::ostream& operator<<(std::ostream& out, const Tetriminos::eColor& color);

struct BlockGenerator {
  explicit BlockGenerator(int seed) { gen.seed(seed); };
  Tetriminos Create();

 private:
  std::mt19937 gen;
};

}  // namespace tetris
