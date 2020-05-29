#include "NintendoClassicScore.h"
#include <array>

namespace tetris {

bool NintendoClassicScore::OnCompletedLine(int nb_line) {
  const std::array<int, 5> gain{0, 40, 100, 300, 1200};
  const int level = Level();

  score += gain.at(nb_line) * level;

  compteted_lines += nb_line;
  return level != Level();
}

std::chrono::milliseconds NintendoClassicScore::DropPeriod(int level_p) const {
  // with block size of 1u, G(level) is  the move for 1 frame at 60Hz

  const std::array<double, 20> G{0,      0.01667, 0.021017, 0.026977, 0.035256, 0.04693, 0.06361,

                                 0.0879, 0.1236,  0.1775,   0.2598,   0.388,    0.59,    0.92,

                                 1.46,   2.36,    3.91,     6.61,     11.43,    20};

  const int level = std::min(std::max(level_p, Level()), 19);

  auto millis = static_cast<int>(1000 / (G.at(level) * 60.0));

  return std::chrono::milliseconds(millis);
};

}  // namespace tetris