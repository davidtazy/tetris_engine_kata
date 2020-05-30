#pragma once
namespace tetris {
enum class UserEvent {
  Left,
  Right,
  Rotate,
  FastDown,
  Pause,
  Resume,
};

struct InputListener {
  virtual void OnLeft() = 0;
  virtual void OnRight() = 0;
  virtual void OnRotate() = 0;
  virtual void OnFastDown() = 0;
  virtual void OnPause() = 0;
  virtual void OnResume() = 0;
};

enum class eInputKey { Left = 0, Right, Rotate, FastDown, Pause, Resume, Count };

struct UserInput {
  void SetListener(InputListener& listener_p) { listener = &listener_p; }

 protected:
  void Fire(eInputKey key) {
    if (!listener)
      return;

    switch (key) {
      case eInputKey::Left:
        listener->OnLeft();
        break;
      case eInputKey::Right:
        listener->OnRight();
        break;
      case eInputKey::Rotate:
        listener->OnRotate();
        break;
      case eInputKey::FastDown:
        listener->OnFastDown();
        break;
      case eInputKey::Pause:
        listener->OnPause();
        break;
      case eInputKey::Resume:
        listener->OnResume();
        break;

      default:
        break;
    }
  }

  InputListener* listener{};
};

}  // namespace tetris