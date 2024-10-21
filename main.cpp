#include <Novice.h>
#include <cstdlib>  // 乱数生成のために追加
#include <ctime>    // 乱数のシードを設定するために追加

const char kWindowTitle[] = "TD2_第一回";

struct Vector2 {
    float x;
    float y;
};

struct Player {
    Vector2 pos;
    Vector2 velocity;
    float radius;
    int jumpCount;  // ジャンプ回数を記録する
};

struct Enemy {
    Vector2 pos;
    float radius;
};



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // 乱数のシードを初期化
    srand(static_cast<unsigned int>(time(nullptr)));

    // 画面変遷
    typedef enum SCENE {
        title,
        description,
        ingame,
        end,
    }SCENE;

    int scene = title;

    // プレイヤーの初期設定
    Player player = { {300.0f, 360.0f}, {0.0f, 0.0f}, 20.0f, 0 };

    // 敵の初期設定
    const int kNumEnemies = 20;  // 敵の数

    Enemy enemies[kNumEnemies];
    for (int i = 0; i < kNumEnemies; i++) {
        enemies[i].pos.x = static_cast<float>(rand() % 5000);  // ランダムなx座標
        enemies[i].pos.y = static_cast<float>(rand() % 720);   // ランダムなy座標
        enemies[i].radius = 20.0f;
    }

    // 重力
    const float Gravity = 0.5f; // 重力の強さ

    //背景の変数
    int back1GH = Novice::LoadTexture("./NoviceResources/back/Background1.png");
    int back2GH = Novice::LoadTexture("./NoviceResources/back/Background2.png");
    int back3GH = Novice::LoadTexture("./NoviceResources/back/Background3.png");
    int back4GH = Novice::LoadTexture("./NoviceResources/back/Background4.png");

    float back1PosX = 0.0f;
    float back2PosX = 1280.0f;
    float back3PosX = 2560.0f;
    float back4posX = 3840.0f;

    //スクロールに関する変数
    float scrollX = 0.0f;
    float scrollEnd1 = 640.0f;
    float scrollEnd2 = 4480.0f;

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // スペースキーの状態を管理
    int spacePressDuration = 0;  // 押下時間
    bool spacePressed = false;   // 押下状態

    // 連打判定のための変数
    int spacePressCount = 0;     // 連打回数
    const int DoubleTapThreshold = 10;  // 連打判定のフレーム数のしきい値
    int lastSpaceReleaseFrame = -DoubleTapThreshold; // 最後にスペースキーを離したフレーム

    // 現在のフレーム数を保持
    int frameCount = 0;

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        ///
        /// ↓更新処理ここから
        ///

        switch (scene){
        //====================
        // タイトルシーン更新
        //====================
        case title: 

            player = { {300.0f, 360.0f}, {0.0f, 0.0f}, 20.0f, 0 };

            for (int i = 0; i < kNumEnemies; i++) {
                enemies[i].pos.x = static_cast<float>(rand() % 5120);  // ランダムなx座標
                enemies[i].pos.y = static_cast<float>(rand() % 720);   // ランダムなy座標
                enemies[i].radius = 20.0f;
            }

            if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
                scene = ingame;
            }

            break;

        //====================
        // 概要説明シーン更新
        //====================
        case description:

            break;

        //====================
        // ゲーム本編更新
        //====================
        case ingame:

            // フレーム数を更新
            frameCount++;

            // 重力の適用（常に下方向に加算）
            player.velocity.y += Gravity;

            // スペースキーが押された場合の処理
            if (keys[DIK_SPACE]) {
                spacePressDuration++;
                if (!spacePressed) {
                    spacePressed = true;
                }
            }
            else {
                if (spacePressed) {
                    // 連打判定処理
                    if (frameCount - lastSpaceReleaseFrame <= DoubleTapThreshold) {
                        spacePressCount++;
                    }
                    else {
                        spacePressCount = 1;
                    }
                    lastSpaceReleaseFrame = frameCount;

                    // ジャンプ回数の制限
                    if (player.jumpCount < 2) {
                        // スペースキーが離された瞬間のジャンプ処理
                        if (spacePressDuration > 0 && spacePressDuration <= 15) {
                            player.velocity.y = -15.0f;
                            player.velocity.x = 5.0f;
                            player.jumpCount++;
                        }
                        // スペース連打の時の処理
                        else if (spacePressDuration > 5) {
                            player.velocity.y = -15.0f;
                            player.velocity.x = -5.0f;
                            player.jumpCount++;
                        }
                    }

                    // 連打による追加アクション（例: プレイヤーの速度を上げる）
                    if (spacePressCount > 2) {
                        player.velocity.y = 30.0f;  // 連打時に速度を上げる
                    }

                    // 押下時間リセット
                    spacePressDuration = 0;
                    spacePressed = false;
                }
            }

            // プレイヤーの位置更新
            player.pos.x += player.velocity.x;
            player.pos.y += player.velocity.y;

            // プレイヤーが地面に着地したらジャンプ回数をリセット
            if (player.pos.y + player.radius > 720) {
                player.pos.y = 720 - player.radius;  // 地面に止まる
                player.velocity.y = 0.0f;            // 垂直方向の速度をリセット
                player.velocity.x = 0.0f;            // 水平方向の速度もリセット
                player.jumpCount = 0;                // ジャンプ回数をリセット
            }

            scrollX = player.pos.x - 640;

            if (scrollX < 0) {
                scrollX = 0;
            }
            else if (scrollX > scrollEnd2 - scrollEnd1) {
                scrollX = scrollEnd2 - scrollEnd1;
            }

            // プレイヤーと敵の当たり判定
            for (int i = 0; i < kNumEnemies; i++) {
                // プレイヤーと敵の距離を計算
                float dx = player.pos.x - enemies[i].pos.x;
                float dy = player.pos.y - enemies[i].pos.y;
                float distance = sqrtf(dx * dx + dy * dy);

                // 衝突判定
                if (distance < player.radius + enemies[i].radius) {
                    // ゲーム終了
                    scene = end;
                    break; // 最初の敵と衝突したらループを抜ける
                }
            }

            // プレイヤーが画面外に出ないようにする
            if (player.pos.x + player.radius > 5120) {
                player.pos.x = 5120 - player.radius;
            }
            else if (player.pos.x - player.radius < 0) {
                player.pos.x = player.radius;
            }

            break;

        //====================
        // 終了シーン更新
        //====================
        case end:
        
            if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
                scene = title;
            }

            break;
        }

        ///
        /// ↑更新処理ここまで
        ///
        
        ///
        /// ↓描画処理ここから
        /// 

        switch (scene){
        //====================
        // タイトルシーン描画
        //====================
        case title:

            break;

        //====================
        // 概要説明シーン描画
        //====================
        case description:

            break;

        //====================
        // ゲーム本編描画
        //====================
        case ingame:

            // 背景の描画
            Novice::DrawSprite((int)back1PosX - (int)scrollX, 0, back1GH, 1.0f, 1.0f, 0.0f, WHITE);
            Novice::DrawSprite((int)back2PosX - (int)scrollX, 0, back2GH, 1.0f, 1.0f, 0.0f, WHITE);
            Novice::DrawSprite((int)back3PosX - (int)scrollX, 0, back3GH, 1.0f, 1.0f, 0.0f, WHITE);
            Novice::DrawSprite((int)back4posX - (int)scrollX, 0, back4GH, 1.0f, 1.0f, 0.0f, WHITE);

            // プレイヤーの描画
            Novice::DrawEllipse((int)(player.pos.x - (int)scrollX), (int)(player.pos.y), (int)player.radius, (int)player.radius, 0.0f, WHITE, kFillModeSolid);

            // 敵の描画
            for (int i = 0; i < kNumEnemies; i++) {
                Novice::DrawEllipse((int)(enemies[i].pos.x - (int)scrollX), (int)(enemies[i].pos.y), (int)enemies[i].radius, (int)enemies[i].radius, 0.0f, RED, kFillModeSolid);
            }

            // 状態表示（デバック用）
            Novice::ScreenPrintf(950, 10, "Space Press Duration: %d", spacePressDuration);
            Novice::ScreenPrintf(950, 30, "Player Position: (%.2f, %.2f)", player.pos.x, player.pos.y);
            Novice::ScreenPrintf(950, 50, "Jump Count: %d", player.jumpCount); // ジャンプ回数表示
            Novice::ScreenPrintf(950, 70, "Space Press Count: %d", spacePressCount); // 連打回数表示

            break;

        //====================
        // 終了シーン描画
        //====================
        case end:

            break;
        }      

        ///
        /// ↑描画処理ここまで
        /// 

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ライブラリの終了
    Novice::Finalize();
    return 0;
}
