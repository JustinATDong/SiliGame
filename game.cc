#include "game.h"
int const ENEMY_SHIELD = 200;
int const SHIELD_CON = 100;
int const LIFE_CON = 200;
void Game::Init() {
  board_.AddMouseEventListener(*this);
  board_.AddAnimationEventListener(*this);
  Plane_.SetX(10);
  Plane_.SetY(board_.GetHeight() - 75);
}

void Game::CreateOpponents() {
  for (int i = 0; i < 5; i++) {
    UFO_.push_back(std::make_unique<Opponent>(50 * i + 15, 50));
  }
}

int Game::HasLife() { return life; }

int Game::GetScore() { return game_score; }

bool Game::HasLost() { return YouLost; }

void Game::UpdateScreen() {
  board_.DrawRectangle(0, 0, 800, 600, 255, 255, 255);
  for (int i = 0; i < UFO_.size(); i++) {
    if (UFO_[i]->GetIsActive() == true) {
      if (YouLost == false) {
        if (game_score > oppo_update) {
          UFO_shield++;
          oppo_update += ENEMY_SHIELD;
        }
        UFO_[i]->Draw(board_);
      }
    }
  }
  for (int i = 0; i < Beam_.size(); i++) {
    if (Beam_[i]->GetIsActive() == true) {
      if (YouLost == false) {
        Beam_[i]->Draw(board_);
      }
    }
  }
  for (int i = 0; i < Bullet_.size(); i++) {
    if (Bullet_[i]->GetIsActive() == true) {
      if (YouLost == false) {
        Bullet_[i]->Draw(board_);
      }
    }
  }
  if (Plane_.GetIsActive() == true || life > 0) {
    if (YouLost == false) {
      if (game_score > shieldupdate) {
        shield++;
        shieldupdate = shieldupdate + SHIELD_CON;
      }
      if (game_score > lifeupdate) {
        life++;
        lifeupdate += LIFE_CON;
      }
      Plane_.Draw(board_);
    }
  } else {
    std::string addition;
    addition = player_lose + std::to_string(game_score);
    board_.DrawText(300, 250, addition, 50, 0, 0, 0);
  }
  std::string s_dis, l_dis;
  s_dis = shield_display + std::to_string(shield);
  l_dis = life_display + std::to_string(life);
  board_.DrawText(670, 10, s_dis, 35, 0, 0, 255);
  board_.DrawText(700, 550, l_dis, 35, 255, 0, 0);
  board_.DrawText(10, 10, std::to_string(game_score), 35, 0, 0, 0);
}

void Game::MoveGameElements() {
  for (int i = 0; i < Bullet_.size(); i++) {
    Bullet_[i]->Move(board_);
  }
  for (int i = 0; i < Beam_.size(); i++) {
    Beam_[i]->Move(board_);
  }
  for (int i = 0; i < UFO_.size(); i++) {
    UFO_[i]->Move(board_);
  }
}

void Game::FilterIntersections() {
  for (int j = 0; j < UFO_.size(); j++) {
    if (Plane_.IntersectsWith(UFO_[j].get()) == true) {
      if(UFO_shield >= 1) {
        UFO_shield--;
      } else if (UFO_shield == 0){
        UFO_[j]->SetIsActive(false);
      }
      if (life > 0) {
        if (shield > 0) {
          shield--;
        } else {
          life--;
        }
      } else {
        Plane_.SetIsActive(false);
      }
    }
  }
  for (int i = 0; i < Beam_.size(); i++) {
    if (Beam_[i]->IntersectsWith(&Plane_) == true) {
      Beam_[i]->SetIsActive(false);
      if (life > 0) {
        if (shield > 0) {
          shield--;
        } else {
          life--;
        }
      } else {
        Plane_.SetIsActive(false);
        YouLost = true;
      }
    }
  }
  for (int a = 0; a < Bullet_.size(); a++) {
    for (int b = 0; b < UFO_.size(); b++) {
      if (Bullet_[a]->IntersectsWith(UFO_[b].get()) == true) {
        Bullet_[a]->SetIsActive(false);
        if (UFO_shield >= 1) {
          UFO_shield--;
        } else if(UFO_shield == 0){
          UFO_[b]->SetIsActive(false);
        }
        if (Plane_.GetIsActive() == true) {
          game_score++;
        }
      }
    }
  }
  for (int a = 0; a < Bullet_.size(); a++) {
    for (int b = 0; b < Beam_.size(); b++) {
      if (Bullet_[a]->IntersectsWith(Beam_[b].get()) == true) {
        Bullet_[a]->SetIsActive(false);
        Beam_[b]->SetIsActive(false);
      }
    }
  }
}
void Game::OnAnimationStep() {
    if (YouLost == false) {
      if (UFO_.size() == 0) {
      CreateOpponents();
    }
  }
  MoveGameElements();
  LaunchProjectiles();
  FilterIntersections();
  RemoveInactive();
  UpdateScreen();
  board_.Flush();
}

void Game::LaunchProjectiles() {
  for (int i = 0; i < UFO_.size(); i++) {
    if (UFO_[i]->LaunchProjectile() != nullptr) {
      std::unique_ptr<OpponentProjectile> pro;
      pro = std::make_unique<OpponentProjectile>();
      pro->SetX(UFO_[i]->GetX());
      pro->SetY(UFO_[i]->GetY());
      Beam_.push_back(std::move(pro));
    }
  }
}

void Game::OnMouseEvent(const graphics::MouseEvent &mouse) {
  if (mouse.GetX() > 0 && mouse.GetY() > 0 &&
      mouse.GetX() < board_.GetWidth() && mouse.GetY() < board_.GetHeight()) {
    Plane_.SetX(mouse.GetX() - 25);
    Plane_.SetY(mouse.GetY() - 25);
  }
  if (mouse.GetMouseAction() == graphics::MouseAction::kPressed) {
    std::unique_ptr<PlayerProjectile> Missle =
        std::make_unique<PlayerProjectile>(mouse.GetX(), mouse.GetY() + 25);
    Bullet_.push_back(std::move(Missle));
  }
  if (mouse.GetMouseAction() == graphics::MouseAction::kDragged) {
    std::unique_ptr<PlayerProjectile> Missle =
        std::make_unique<PlayerProjectile>(mouse.GetX(), mouse.GetY() + 25);
    Bullet_.push_back(std::move(Missle));
  }
}

void Game::RemoveInactive() {
  for (int i = Bullet_.size() - 1; i >= 0; i--) {
    if (Bullet_[i]->GetIsActive() == false) {
      Bullet_.erase(Bullet_.begin() + i);
    }
  }
  for (int i = Beam_.size() - 1; i >= 0; i--) {
    if (Beam_[i]->GetIsActive() == false) {
      Beam_.erase(Beam_.begin() + i);
    }
  }
  for (int i = UFO_.size() - 1; i >= 0; i--) {
    if (!UFO_[i]->GetIsActive()) {
      UFO_.erase(UFO_.begin() + i);
    }
  }
}
