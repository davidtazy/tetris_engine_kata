#pragma once
#include <chrono>

namespace tetris {

//! every flavour has its scoring system ...
//! this interface should allow to implements most of them
//!
struct IScore {
  virtual void OnNewTetriminos() = 0;

  //! return true if level changed
  virtual bool OnCompletedLine(int nb_line) = 0;
  virtual void OnPerfectClear() = 0;
  virtual void OnSoftDrop() = 0;

  virtual int Score() const = 0;

  // responsible to define level
  virtual int Level() const = 0;

  virtual int CompletedLines() const = 0;

  //!@param level = 0 mean, with current level
  virtual std::chrono::milliseconds DropPeriod(int level = 0) const = 0;
};

};  // namespace tetris