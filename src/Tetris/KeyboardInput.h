#pragma once

#include <Tetris/IUserInput.h>
#include <any>
#include <chrono>

namespace tetris {

class KeyBoardInputs : public UserInput {
  friend class KeyBoardInputsBuilder;

  std::vector<std::any> keys;
  std::chrono::milliseconds repeat_delay;

  std::any& Key(eInputKey k) { return keys[static_cast<int>(k)]; }

  template <typename T>
  bool Is(const T& user_key_p, const std::any& key) const {
    if (key.has_value()) {
      try {
        T v = std::any_cast<T>(key);
        return v == user_key_p;
      } catch (std::bad_cast e) {
      }
    }
    return false;
  }

  template <typename T>
  eInputKey GetKeyOrThrowIfNotAssigned(const T& user_key) const {
    for (int i = 0; i < static_cast<int>(eInputKey::Count); i++) {
      if (Is(user_key, keys.at(i))) {
        eInputKey k = static_cast<eInputKey>(i);
        return k;
      }
    }
    throw std::runtime_error("user key is not assigned");
  }

 public:
  KeyBoardInputs() : keys(static_cast<int>(eInputKey::Count)) {}
  virtual ~KeyBoardInputs() = default;

  template <typename T>
  void OnKeyPressed(T user_key) {
    auto key = GetKeyOrThrowIfNotAssigned(user_key);

    Fire(key);
  }

  template <typename T>
  void OnKeyReleased(T key) {}
};

struct KeyBoardInputsBuilder {
  template <typename T>
  KeyBoardInputsBuilder& AssignLeft(T user_key) {
    return Assign(eInputKey::Left, user_key);
  }
  template <typename T>
  KeyBoardInputsBuilder& AssignRight(T user_key) {
    return Assign(eInputKey::Right, user_key);
  }

  template <typename T>
  KeyBoardInputsBuilder& AssignRotate(T user_key) {
    return Assign(eInputKey::Rotate, user_key);
  }

  template <typename T>
  KeyBoardInputsBuilder& AssignMoveDown(T user_key) {
    return Assign(eInputKey::FastDown, user_key);
  }

  template <typename T>
  KeyBoardInputsBuilder& AssignPause(T user_key) {
    return Assign(eInputKey::Pause, user_key);
  }

  template <typename T>
  KeyBoardInputsBuilder& AssignResume(T user_key) {
    return Assign(eInputKey::Resume, user_key);
  }

  KeyBoardInputsBuilder& EnableRepeatDelay(std::chrono::milliseconds delay) {
    inputs.repeat_delay = delay;
    return *this;
  }

  KeyBoardInputs Build() { return inputs; };

 private:
  template <typename T>
  KeyBoardInputsBuilder& Assign(eInputKey key_p, T& user_key) {
    ThrowIfKeysNotUnique(user_key);
    inputs.Key(key_p) = user_key;
    return *this;
  }

  template <typename T>
  void ThrowIfKeysNotUnique(T user_key_p) {
    for (const auto& key : inputs.keys)
      if (inputs.Is(user_key_p, key)) {
        throw std::runtime_error("same key used for different actions");
      }
  }

  KeyBoardInputs inputs;
};

}  // namespace tetris
