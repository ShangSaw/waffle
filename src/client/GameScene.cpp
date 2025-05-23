#include "nlohmann/json.hpp"
#include "GameScene.hpp"
using json = nlohmann::json;

static constexpr float MOVE_SPEED = 5.0f;
static constexpr float PLAYER_WH = 80.0f;
static constexpr Coordonnees worldMin{ 0,0 }, worldMax{ 2000 - 80,2000 - 80 };

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
{
    skin_path = "";

    // bind network callbacks
    ctx.network.onPlayerConnect = [this](int id, float x, float y) {
        if (id != me_.id)
            players_[id] = { id, {x, y, PLAYER_WH, PLAYER_WH}, {0,0,0,0} };
        };

    ctx.network.onPlayerUpdate = [this](int id, float x, float y) {
        players_[id].r.x = x;
        players_[id].r.y = y;
        };

    ctx.network.onPlayerDisconnect = [this](int id) {
        players_.erase(id);
        };

    ctx.network.onOwnId = [this](int id) {
        std::cout << "changed id from" << me_.id << std::endl;
        me_.id = id;
        std::cout << "to " << id << std::endl;

        // download custom skin
        skin_path = "skins/";

        if (!std::filesystem::exists(skin_path)) {
            std::filesystem::create_directory(skin_path);
        }

        skin_path.append(std::to_string(id) + ".png");
        ctx_.resources.downloadPNG(
            "https://pngimg.com/uploads/anime_girl/anime_girl_PNG93.png",
            skin_path);
        SDL_Texture* skin = ctx_.resources.get(skin_path);

        json msgJson = {
            {"type", PacketTypes::SKIN_LINK},
            {"id", id},
            {"link", "https://pngimg.com/uploads/anime_girl/anime_girl_PNG93.png"},
        };

        std::cout << msgJson.dump() << std::endl;
        ctx_.network.sendPacket(msgJson.dump());
        };

    ctx_.network.onSkinLink = [this](int id, std::string link) {
        std::cout << "received skin link of player: " << id << std::endl;
        skin_path = "skins/";

        if (!std::filesystem::exists(skin_path)) {
            std::filesystem::create_directory(skin_path);
        }

        skin_path.append(std::to_string(id) + ".png");
        ctx_.resources.downloadPNG(
            "https://pngimg.com/uploads/anime_girl/anime_girl_PNG93.png",
            skin_path);
        SDL_Texture* skin = ctx_.resources.get(skin_path);
        };
}

void GameScene::init()
{
    me_.r.w = PLAYER_WH;
    me_.r.h = PLAYER_WH;
    me_.r.x = 100;
    me_.r.y = 100;
}

void GameScene::processInput() {
    ctx_.input.pollEvents();
    if (ctx_.input.quitting()) quit_ = true;
}

void GameScene::update()
{
    ctx_.network.poll();

    Coordonnees m = ctx_.input.movement();
    me_.r.x = std::clamp(me_.r.x + m.x * MOVE_SPEED, worldMin.x, worldMax.x);
    me_.r.y = std::clamp(me_.r.y + m.y * MOVE_SPEED, worldMin.y, worldMax.y);

    if (me_.id == -1) {
        return;
    }

    if ((m.x != 0 || m.y != 0)) {
        ctx_.network.sendPosition(me_.id, me_.r.x, me_.r.y);
    }

}

void GameScene::render()
{

    ctx_.renderer.clear();
    ctx_.renderer.setCamera(me_.r);
    // draw self
    ctx_.renderer.draw(me_.r, ctx_.resources.get("skins/" + std::to_string(me_.id) + ".png"));
    // draw others
    for (auto& [_, p] : players_) {
        ctx_.renderer.draw(p.r, ctx_.resources.get("skins/" + std::to_string(p.id) + ".png"));
    }

    // potential code for rendering ImGui

    ctx_.renderer.present();
}

bool GameScene::wantsSceneChange() const
{
    return false;
}

std::unique_ptr<Scene> GameScene::nextScene()
{
    return nullptr;
}
