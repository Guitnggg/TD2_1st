#include <Novice.h>

const char kWindowTitle[] = "学籍番号";

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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // プレイヤーの初期設定
    Player player = { {100.0f, 360.0f}, {0.0f, 0.0f}, 20.0f, 0 };

    // 重力
    const float Gravity = 0.5f; // 重力の強さ

    //スクロールに関する変数
    //背景の変数
	int back1GH = Novice::LoadTexture("./NoviceResources/back/Background1.png");
	int back2GH = Novice::LoadTexture("./NoviceResources/back/Background2.png");
	int back3GH = Novice::LoadTexture("./NoviceResources/back/Background3.png");
	int back4GH = Novice::LoadTexture("./NoviceResources/back/Background4.png");

	float back1PosX = 0.0f;
	float back2PosX = 1280.0f;
	float back3PosX = 2560.0f;
	float back4posX = 3840.0f;

    float scrollX;
    float scrollEnd1 = 640.0f;
    float scrollEnd2 = 4480.0f;

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // スペースキーの状態を管理
    int spacePressDuration = 0;  // 押下時間
    bool spacePressed = false;   // 押下状態

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        /// ↓更新処理ここから

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
            if (spacePressed && player.jumpCount < 2) {
                // スペースキーが離された瞬間のジャンプ処理
                if (spacePressDuration > 0 && spacePressDuration <= 15) {
                    player.velocity.y = -15.0f;
                    player.velocity.x = 10.0f;
                    player.jumpCount++;
                }
                else if (spacePressDuration > 15) {
                    player.velocity.y = -15.0f;
                    player.velocity.x = -10.0f;
                    player.jumpCount++;
                }
            }
            // 押下時間リセット
            spacePressDuration = 0;
            spacePressed = false;
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

        // プレイヤーが画面外に出ないようにする
        if (player.pos.x + player.radius > 5120) {
            player.pos.x = 5120 - player.radius;
        }
        else if (player.pos.x - player.radius < 0) {
            player.pos.x = player.radius;
        }

        /// ↑更新処理ここまで

        /// ↓描画処理ここから

        Novice::DrawSprite((int)back1PosX - (int)scrollX, 0, back1GH, 1.0f, 1.0f, 0.0f, WHITE);
        Novice::DrawSprite((int)back2PosX - (int)scrollX, 0, back2GH, 1.0f, 1.0f, 0.0f, WHITE);
        Novice::DrawSprite((int)back3PosX - (int)scrollX, 0, back3GH, 1.0f, 1.0f, 0.0f, WHITE);
        Novice::DrawSprite((int)back4posX - (int)scrollX, 0, back4GH, 1.0f, 1.0f, 0.0f, WHITE);

        // プレイヤーの描画
        Novice::DrawEllipse((int)(player.pos.x-(int)scrollX), (int)(player.pos.y), (int)player.radius, (int)player.radius, 0.0f, WHITE, kFillModeSolid);

        // 状態表示
        Novice::ScreenPrintf(950, 10, "Space Press Duration: %d", spacePressDuration);
        Novice::ScreenPrintf(950, 30, "Player Position: (%.2f, %.2f)", player.pos.x, player.pos.y);
        Novice::ScreenPrintf(950, 50, "Jump Count: %d", player.jumpCount); // ジャンプ回数表示

        /// ↑描画処理ここまで

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
