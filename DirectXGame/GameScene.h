#pragma once
#include "KamataEngine.h"
#include "Player.h"

class GameScene {
public:
    //========================================
    // 初期化処理
    //========================================

    // 初期化
    void Initialize();

    //========================================
    // 更新処理
    //========================================

    // 更新
    void Update();

    //========================================
    // 描画処理
    //========================================

    // 描画
    void Draw();

private:
    // テクスチャハンドル
    uint32_t textureHandle_ = 0;

    // スプライト
    KamataEngine::Model* model_ = nullptr;

    // ワールドトランスフォーム
    KamataEngine::WorldTransform worldTransform_;

    // カメラ
    KamataEngine::Camera* camera_;

    // 自キャラ
    Player* player_ = nullptr;
};
