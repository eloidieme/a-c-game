#include "raylib.h"
#include <stdlib.h>

#define PLAYER_LIFES 5
#define BRICK_LINES 5
#define BRICKS_PER_LINE 20

#define BRICKS_POSITION_Y 50

typedef enum GameScreen { LOGO,
                          TITLE,
                          GAMEPLAY,
                          ENDING } GameScreen;

typedef struct Player Player;
typedef struct Ball Ball;
typedef struct Brick Brick;

struct Player {
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    Rectangle bounds;
    int lifes;
};

struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
};

struct Brick {
    Vector2 position;
    Vector2 size;
    Rectangle bounds;
    int resistance;
    bool active;
};

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "PROJECT: BLOCKS GAME");

    GameScreen screen = LOGO;

    unsigned framesCounter = 0;
    int8_t gameResult = -1;  // 0 - Loose, 1 - Win, -1 - Not defined
    bool gamePaused = false;

    Player player = {0};
    Ball ball = {0};
    Brick bricks[BRICK_LINES][BRICKS_PER_LINE] = {0};

    player.position = (Vector2){(float)screenWidth / 2, (float)screenHeight * 7 / 8};
    player.speed = (Vector2){8.0f, 0.0f};
    player.size = (Vector2){100, 24};
    player.lifes = PLAYER_LIFES;

    ball.radius = 10.0f;
    ball.active = false;
    ball.position = (Vector2){player.position.x + player.size.x / 2,
                              player.position.y - ball.radius * 2};
    ball.speed = (Vector2){4.0f, 4.0f};

    for (int j = 0; j < BRICK_LINES; j++) {
        for (int i = 0; i < BRICKS_PER_LINE; i++) {
            bricks[j][i].size = (Vector2){(float)screenWidth / BRICKS_PER_LINE, 20};
            bricks[j][i].position = (Vector2){
                i * bricks[j][i].size.x,
                j * bricks[j][i].size.y + BRICKS_POSITION_Y};
            bricks[j][i].bounds = (Rectangle){bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y};
            bricks[j][i].active = true;
        }
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        switch (screen) {
        case LOGO: {
            framesCounter++;

            if (framesCounter > 180) {
                screen = TITLE;  // change to TITLE screen after 3 seconds
                framesCounter = 0;
            }
        } break;
        case TITLE: {
            framesCounter++;

            if (IsKeyPressed(KEY_ENTER))
                screen = GAMEPLAY;
        } break;
        case GAMEPLAY: {
            if (IsKeyPressed('P'))
                gamePaused = !gamePaused;

            if (!gamePaused) {
                if (IsKeyDown(KEY_LEFT))
                    player.position.x -= player.speed.x;
                if (IsKeyDown(KEY_RIGHT))
                    player.position.x += player.speed.x;

                if ((player.position.x) <= 0)
                    player.position.x = 0;
                if ((player.position.x + player.size.x) >= screenWidth)
                    player.position.x = screenWidth - player.size.x;

                player.bounds = (Rectangle){player.position.x, player.position.y, player.size.x, player.size.y};

                if (ball.active) {
                    ball.position.x += ball.speed.x;
                    ball.position.y += ball.speed.y;

                    if (((ball.position.x + ball.radius) >= screenWidth) || ((ball.position.x - ball.radius) <= 0))
                        ball.speed.x *= -1;
                    if ((ball.position.y - ball.radius) <= 0)
                        ball.speed.y *= -1;

                    if (CheckCollisionCircleRec(ball.position, ball.radius, player.bounds)) {
                        ball.speed.y *= -1;
                        ball.speed.x = (ball.position.x - (player.position.x + player.size.x / 2)) / player.size.x * 5.0f;
                    }

                    for (int j = 0; j < BRICK_LINES; j++) {
                        for (int i = 0; i < BRICKS_PER_LINE; i++) {
                            if (bricks[j][i].active && (CheckCollisionCircleRec(ball.position, ball.radius, bricks[j][i].bounds))) {
                                bricks[j][i].active = false;
                                ball.speed.y *= -1;

                                break;
                            }
                        }
                    }

                    if ((ball.position.y + ball.radius) >= screenHeight) {
                        ball.position.x = player.position.x + player.size.x / 2;
                        ball.position.y = player.position.y - ball.radius - 1.0f;
                        ball.speed = (Vector2){0, 0};
                        ball.active = false;

                        player.lifes--;
                    }

                    if (player.lifes < 0) {
                        screen = ENDING;
                        player.lifes = 5;
                        framesCounter = 0;
                    }
                } else {
                    ball.position.x = player.position.x + player.size.x / 2;

                    if (IsKeyPressed(KEY_SPACE)) {
                        ball.active = true;
                        ball.speed = (Vector2){0, -5.0f};
                    }
                }
            }
        } break;
        case ENDING: {
            framesCounter++;

            if (IsKeyPressed(KEY_ENTER))
                screen = TITLE;
        } break;
        default:
            break;
        }

        // DRAW
        BeginDrawing();

        ClearBackground(RAYWHITE);

        switch (screen) {
        case LOGO: {
            DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
        } break;
        case TITLE: {
            DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
            if ((framesCounter / 30) % 2 == 0)
                DrawText("PRESS [ENTER] to START", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] to START", 20) / 2, GetScreenHeight() / 2 + 60, 20, DARKGRAY);
        } break;
        case GAMEPLAY: {
            DrawRectangle(player.position.x, player.position.y, player.size.x, player.size.y, BLACK);
            DrawCircle(ball.position.x, ball.position.y, ball.radius, MAROON);

            for (int j = 0; j < BRICK_LINES; j++) {
                for (int i = 0; i < BRICKS_PER_LINE; i++) {
                    if (bricks[j][i].active) {
                        if ((i + j) % 2 == 0)
                            DrawRectangle(bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y, GRAY);
                        else
                            DrawRectangle(bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y, DARKGRAY);
                    }
                }
            }

            for (int i = 0; i < player.lifes; i++)
                DrawRectangle(20 + 40 * i, screenHeight - 30, 35, 10, LIGHTGRAY);

            if (gamePaused)
                DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 + 60, 40, GRAY);
        } break;
        case ENDING: {
            DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);

            if ((framesCounter / 30) % 2 == 0)
                DrawText("PRESS [ENTER] to PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] to PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 + 80, 20, GRAY);
        } break;
        default:
            break;
        }

        EndDrawing();
    }

    // De-init

    // Unload any loaded resources
    CloseWindow();

    return EXIT_SUCCESS;
}
