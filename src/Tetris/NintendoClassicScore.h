#pragma once

#include <chrono>
#include "Tetris/IScore.h"

namespace tetris {

//! @ref https://tetris.wiki/Scoring#Original_Nintendo_scoring_system
class NintendoClassicScore : public IScore {
  int score{};
  int nb_lines{};
  int compteted_lines{};

 public:
  void OnNewTetriminos() override { score++; };

  //! return true if level changed
  bool OnCompletedLine(int nb_line) override;

  void OnPerfectClear() override{};

  void OnSoftDrop() override { score++; };

  int Score() const override { return score; };

  int Level() const override { return 1 + compteted_lines / 10; };

  int CompletedLines() const override { return compteted_lines; };

  std::chrono::milliseconds DropPeriod(int level = 0) const override;
};

}  // namespace tetris