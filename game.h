#include "cpputils/graphics/image_event.h"
#include "opponent.h"
#include "player.h"

#ifndef GAME_H
#define GAME_H

class Game : public graphics::AnimationEventListener,
             public graphics::MouseEventListener {
 public:
  Game() { board_.Initialize(800, 600); }

  Game(int width, int height) { board_.Initialize(width, height); }
  graphics::Image &GetGameScreen() { return board_; }
  std::vector<std::unique_ptr<PlayerProjectile>> &GetPlayerProjectiles() {
    return Bullet_;
  }
  std::vector<std::unique_ptr<OpponentProjectile>> &GetOpponentProjectiles() {
    return Beam_;
  }
  std::vector<std::unique_ptr<Opponent>> &GetOpponents() { return UFO_; }
  Player &GetPlayer() { return Plane_; }
  void CreateOpponents();
  void Init();
  void UpdateScreen();
  void Start(graphics::Image &game) { game.ShowUntilClosed(); }
  void MoveGameElements();
  void FilterIntersections();
  void OnAnimationStep();
  void OnMouseEvent(const graphics::MouseEvent &mouseObject);
  void LaunchProjectiles();
  void RemoveInactive();
  int GetScore();
  bool HasLost();
  int HasLife();

 private:
  graphics::Image board_;
  std::vector<std::unique_ptr<PlayerProjectile>> Bullet_;
  std::vector<std::unique_ptr<OpponentProjectile>> Beam_;
  std::vector<std::unique_ptr<Opponent>> UFO_;
  Player Plane_;
  int game_score = 0;
  bool YouLost = false;
  int oppo_life = 0;
  int oppo_update = 100;
  int UFO_shield = 1;
  std::string shield_display = "Shield: ";
  std::string life_display = "Life: ";
  int shieldupdate = 100;
  int shield = 1;
  int lifeupdate = 400;
  int life = 3;
 std::string player_lose = "YOU LOST :(\n score: ";
};
#endif
