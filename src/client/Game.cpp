#include "Game.hpp"

static constexpr float MOVE_SPEED = 5.0f;
static constexpr float PLAYER_WH = 80.0f;
static constexpr Coordonnees worldMin{ 0,0 }, worldMax{ 2000 - 80,2000 - 80 };

Game::Game(const std::string& server, uint16_t port)
    : renderer_(1280, 720)
    , resources_(renderer_.renderer_)
    , network_(server, port)
{
    skin_path = "";

    // bind network callbacks
    network_.onPlayerConnect = [this](int id, float x, float y) {
        if (id != me_.id)
            players_[id] = { id, {x, y, PLAYER_WH, PLAYER_WH}, {0,0,0,0}};
    };

    network_.onPlayerUpdate = [this](int id, float x, float y) {
        players_[id].r.x = x;
        players_[id].r.y = y;
    };

    network_.onPlayerDisconnect = [this](int id) {
        players_.erase(id);
    };

    network_.onOwnId = [this](int id) {
        std::cout << "changed id from" << me_.id << std::endl;
        me_.id = id;
        std::cout << "to " << id << std::endl;

        // download custom skin
        skin_path = "skins/";

        if (!std::filesystem::exists(skin_path)) {
            std::filesystem::create_directory(skin_path);
        }

        skin_path.append(std::to_string(id) + ".png");
        resources_.downloadPNG(
            "https://pngimg.com/uploads/anime_girl/anime_girl_PNG93.png",
            skin_path);
        SDL_Texture* skin = resources_.get(skin_path);

        std::string msg = std::to_string(PacketTypes::SKIN_LINK) + "|" + std::to_string(id)+ "|" + "https://pngimg.com/uploads/anime_girl/anime_girl_PNG93.png";
        std::cout << msg << std::endl;
        network_.sendPacket(msg);
    };

    network_.onSkinLink = [this](int id, std::string sl) {
        std::cout << "received skin link of player: " << id << std::endl;
        skin_path = "skins/";

        if (!std::filesystem::exists(skin_path)) {
            std::filesystem::create_directory(skin_path);
        }

        skin_path.append(std::to_string(id) + ".png");
        resources_.downloadPNG(
            "https://pngimg.com/uploads/anime_girl/anime_girl_PNG93.png",
            skin_path);
        SDL_Texture* skin = resources_.get(skin_path);
    };
}


void Game::render() {
    renderer_.clear();
    renderer_.setCamera(me_.r);
    // draw self
    renderer_.draw(me_.r, resources_.get("skins/" + std::to_string(me_.id) + ".png"));
    // draw others
    for (auto& [_, p] : players_) {
        renderer_.draw(p.r, resources_.get("skins/" + std::to_string(p.id) + ".png"));
    }
    renderer_.present();
}

void Game::run() {
    me_.r.w = PLAYER_WH;
    me_.r.h = PLAYER_WH;
    me_.r.x = 100;
    me_.r.y = 100;


    while (running_) {
        processInput();
        update();
        network_.poll();
        render();

        SDL_Delay(16);
    }
}

void Game::processInput() {
    input_.pollEvents();
    if (input_.quitting()) running_ = false;
    SDL_SetWindowFullscreen(renderer_.window_, input_.toggleFS());
}

void Game::update() {
    Coordonnees m = input_.movement();
    me_.r.x = std::clamp(me_.r.x + m.x * MOVE_SPEED, worldMin.x, worldMax.x);
    me_.r.y = std::clamp(me_.r.y + m.y * MOVE_SPEED, worldMin.y, worldMax.y);

    if (me_.id == -1) {
        return;
    }


    network_.sendPosition(me_.id, me_.r.x, me_.r.y);

    me_.dr.x = me_.r.x - camera_.x;
    me_.dr.y = me_.r.y - camera_.y;

    for (auto& [id, j] : players_) {
        j.dr.x = j.r.x - camera_.x;
        j.dr.y = j.r.y - camera_.y;
        j.dr.w = j.r.w;
        j.dr.h = j.r.h;
    }

}