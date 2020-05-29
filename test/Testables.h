#include <Tetris/Tetris.h>

using namespace tetris;

struct TestableTimer : public ITimer {
  int start_call{};
  void Start(const std::chrono::milliseconds& period) {
    start_call++;
    started = true;
  }
  void Stop() { started = false; }
  using ITimer::Step;
};

struct TetrisTestable : public Tetris {
  using Tetris::Tetris;
  void OnLeft() override {
    Tetris::OnLeft();
    input_event++;
  }
  void OnRight() override {
    Tetris::OnRight();
    input_event += 10;
  }
  void OnRotate() override {
    Tetris::OnRotate();
    input_event += 100;
  }
  void OnFastDown() override {
    Tetris::OnFastDown();
    input_event += 1000;
  }
  void OnPause() override {
    Tetris::OnPause();
    input_event += 10000;
  }
  void OnResume() override {
    Tetris::OnResume();
    input_event += 100000;
  }
  void OnTimerEvent(const ITimer& timer) override {
    Tetris::OnTimerEvent(timer);
    timer_event++;
  }

  void AddStaleBlocks(const std::vector<Pos>& blocks) {
    for (auto pos : blocks)
      AddStaleBlock(Block{pos, Tetriminos::eColor::Blue});
  }

  using Tetris::AddStaleBlock;
  using Tetris::ApplyGravity;
  using Tetris::Land;
  using Tetris::LoadNext;
  using Tetris::RemoveAllBlocksInLine;
  using Tetris::SetCurrent;

  int input_event{};
  int timer_event{};
};

struct DummyScore : IScore {
  int compteted_lines{};
  void OnNewTetriminos() override{};

  //! return true if level changed
  bool OnCompletedLine(int nb_line) override {
    int level = Level();
    compteted_lines += nb_line;
    return level != Level();
  };
  void OnPerfectClear() override{};
  void OnSoftDrop() override{};

  int Score() const override { return 0; };

  int Level() const override { return 1 + compteted_lines / 10; };

  int CompletedLines() const override { return compteted_lines; };

  std::chrono::milliseconds DropPeriod() const override { return std::chrono::seconds{1}; };
};

static void CreateCompletedLine(TetrisTestable& game, int height) {
  for (int i = 0; i < game.Width(); i++)
    game.AddStaleBlocks({Pos{i, height}});
}

//! create block at line @param heigh
//! @param pattern start and finish with # (walls) and '.' is empty, other is block
//! @example pattern= "#xxx...xxx#" 3 blocks on the left and on the right void in the middle
static void CreateLine(TetrisTestable& game, int height, std::string pattern) {
  REQUIRE(pattern.size() == game.Width() + 2);
  REQUIRE(pattern.front() == '#');
  REQUIRE(pattern.back() == '#');

  pattern = pattern.substr(1, game.Width());

  for (int i = 0; i < pattern.size(); i++) {
    char c = pattern.at(i);
    if (c != '.')
      game.AddStaleBlocks({Pos{i, height}});
  }
}

struct TestableGenerator : ITetriminosGenerator {
  std::list<Tetriminos> buf;

  Tetriminos Create() override {
    REQUIRE(buf.size());
    auto t = buf.front();
    buf.pop_front();
    return t;
  }
};

//! draw tetris state
static std::string dump(const Tetris& game) {
  std::string init_line(2 + game.Width(), '.');
  std::vector<std::string> out(game.Height() + 1, init_line);

  for (auto p : game.LeftWall()) {
    out[p.y][p.x + 1] = '#';
  }
  for (auto p : game.RightWall()) {
    out[p.y][p.x + 1] = '#';
  }
  for (auto p : game.Floor()) {
    out[p.y][p.x + 1] = '#';
  }
  for (auto b : game.StaleBlocks()) {
    auto p = b.pos;
    out[p.y][p.x + 1] = 'x';
  }

  for (auto p : game.Current().BlocksAbsolutePosition()) {
    out[p.y][p.x + 1] = '@';
  }

  return std::accumulate(out.begin(), out.end(), std::string{},
                         [](std::string acc, std::string line) {
                           acc += line;
                           acc += '\n';
                           return acc;
                         });
}