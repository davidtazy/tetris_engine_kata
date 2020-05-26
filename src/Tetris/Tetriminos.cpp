#include "Tetriminos.h"
#include <algorithm>
#include <stdexcept>
namespace tetris {

static std::array<std::string, Tetriminos::BlockTypeCount()> CollectionStrings{
    "I", "O", "T", "L", "J", "Z", "S",
};

Tetriminos::eType FromString(std::string type_p) {
  auto it = std::find(CollectionStrings.begin(), CollectionStrings.end(), type_p);
  if (it == CollectionStrings.end()) {
    throw std::runtime_error("Tetriminos(std::string type_p) , invalid input parameter");
  }

  return static_cast<Tetriminos::eType>(std::distance(CollectionStrings.begin(), it));
}

Tetriminos::Tetriminos(std::string type_p) : Tetriminos(FromString(type_p)) {}

Tetriminos::Tetriminos(eType type_p) : type(type_p) {
  blocks = [this]() {
    switch (type) {
      case eType::I:
        return std::vector<Pos>{{0, 0}, {1, 0}, {2, 0}, {3, 0}};
      case eType::O:
        return std::vector<Pos>{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
      case eType::T:
        return std::vector<Pos>{{-1, 0}, {0, 0}, {1, 0}, {0, -1}};
      case eType::L:
        return std::vector<Pos>{{0, 0}, {0, 1}, {0, 2}, {1, 2}};
      case eType::J:
        return std::vector<Pos>{{0, 0}, {0, 1}, {0, 2}, {-1, 2}};
      case eType::Z:
        return std::vector<Pos>{{-1, 0}, {0, 0}, {0, 1}, {1, 1}};
      case eType::S:
        return std::vector<Pos>{{-1, 1}, {0, 1}, {0, 0}, {1, 0}};
      default:
        throw std::runtime_error("cannot create blocks of this unkinw type");
    };
  }();
}

bool Collision(const std::vector<Pos>& a, const std::vector<Pos>& b) {
  return std::any_of(a.begin(), a.end(), [&](const Pos& pos) {
    return std::any_of(b.begin(), b.end(), [&pos](const Pos& p) { return p == pos; });
  });
}

void Tetriminos::Rotate() {
  // x2=cosβx1−sinβy1
  // y2=sinβx1+cosβy1
  std::for_each(blocks.begin(), blocks.end(), [](Pos& pos) {
    auto x = pos.x;
    pos.x = -pos.y;
    pos.y = x;
  });
}

Tetriminos::eColor Tetriminos::ToColor(eType type_p) {
  static_assert(static_cast<int>(eType::Count) == static_cast<int>(eColor::Count));
  if (type_p >= eType::Count)
    throw std::runtime_error("Tetriminos::ToColor , invalid input parameter");
  return static_cast<eColor>(static_cast<int>(type_p));
}

constexpr Tetriminos::Collection Tetriminos::TypeCollection() {
  return {eType::I, eType::J, eType::L, eType::O, eType::S, eType::T, eType::Z};
}

std::ostream& operator<<(std::ostream& out, const Tetriminos::eType& type) {
  return out << CollectionStrings.at(static_cast<size_t>(type));
}

std::ostream& operator<<(std::ostream& out, const Tetriminos::eColor& color) {
  static std::array<std::string, static_cast<size_t>(Tetriminos::eColor::Count)> types{
      "Cyan", "Yellow", "Purple", "Orange", "Blue", "Red", "Green",
  };
  return out << types.at(static_cast<size_t>(color));
}

std::ostream& operator<<(std::ostream& out, const Pos& pos) {
  return out << "(" << pos.x << ";" << pos.y << ")";
}

///// Generator
Tetriminos BlockGenerator::Create() {
  std::vector<Tetriminos::eType> sample;

  auto collection = Tetriminos::TypeCollection();
  std::sample(collection.begin(), collection.end(), std::back_inserter(sample), 1, gen);
  return Tetriminos(sample.front());
};

}  // namespace tetris
