#include "raylib.h"
#include <math.h>

int window_width = 1280;
int window_height = 720;

#define COLOR_PLAYER      (Color){ 0, 0, 0}
#define COLOR_BACKGROUND  (Color){ 37, 58, 94, 255 }
#define COLOR_VOID        (Color){ 151, 89, 191, 255 }
#define COLOR_FIRE        (Color){ 218, 134, 62, 255 }
#define COLOR_AIR         (Color){ 168, 181, 178, 255 }
#define COLOR_WATER       (Color){ 79, 143, 186, 255 }
#define COLOR_EARTH       (Color){ 173, 119, 87, 255 }
// :colors

typedef struct Entity {
  int id;
  int arch;
  Texture2D* sprite;
  Color color;
  Vector2 position;
  int facing;
  int anim;
  // :entity
} Entity;

typedef enum Sprite {
  SPRITE_NIL = 0,
  SPRITE_PLAYER,
  SPRITE_PLAYER_CROUCH,
  SPRITE_VOID,
  SPRITE_FIRE,
  SPRITE_AIR,
  SPRITE_WATER,
  SPRITE_EARTH,
  SPRITE_TREE,
  SPRITE_ORE,
  SPRITE_PUDDLE,
  SPRITE_ORB,
  // :sprite
  SPRITE_MAX,
} Sprite;

typedef enum JumpState {
  JUMP_DISABLED,
  JUMP_AVAILABLE,
  JUMP_ASCENDING,
  JUMP_DESCENDING,
  JUMP_FASTFALL,
} JumpState;

// :debug
bool debug = true;
bool ui_open = false;
int ui_menu = 0; // character, map, crafting, skill, etc

// :tweaks
const float run_speed = 100.0f;

float velocity;
int move_dir;
bool decelerating;
int jump_state;
bool jumping;
int jump_count;

Texture2D sprite[SPRITE_MAX];
Camera2D camera;

int main () {
  InitWindow(window_width, window_height, "ooga");
  SetTargetFPS(60);

  sprite[SPRITE_PLAYER] = LoadTexture("res/sprites/player.png");
  sprite[SPRITE_PLAYER_CROUCH] = LoadTexture("res/sprites/player_crouch.png");
  sprite[SPRITE_VOID] = LoadTexture("res/sprites/void.png");
  sprite[SPRITE_FIRE] = LoadTexture("res/sprites/fire.png");
  sprite[SPRITE_AIR] = LoadTexture("res/sprites/air.png");
  sprite[SPRITE_WATER] = LoadTexture("res/sprites/water.png");
  sprite[SPRITE_EARTH] = LoadTexture("res/sprites/earth.png");
  sprite[SPRITE_TREE] = LoadTexture("res/sprites/tree.png");
  sprite[SPRITE_ORE] = LoadTexture("res/sprites/ore.png");
  sprite[SPRITE_PUDDLE] = LoadTexture("res/sprites/puddle.png");
  sprite[SPRITE_ORB] = LoadTexture("res/sprites/orb.png");

  camera.rotation = 0.0f;
  camera.offset = (Vector2) {window_width/2.0f, window_height/2.0f};
  camera.zoom = 2.0f;

  Entity player = {
    .position =  (Vector2) {100, 100},
    .sprite = &sprite[SPRITE_PLAYER],
  };

 Entity en[100] = {
 };

 for (int i = 0; i < 100; i++) {
   en[i].position.x = i * 40;
   en[i].position.y = 96;
   en[i].sprite = &sprite[SPRITE_TREE];
   en[i].color = GREEN;
 };
  
  while (!WindowShouldClose()) {
    float delta_t = GetFrameTime();

    if (IsKeyPressed(KEY_ESCAPE)) WindowShouldClose();
    if (IsKeyPressed(KEY_P)) debug = !debug;
    if (IsKeyPressed(KEY_TAB)) ui_open = !ui_open ;

    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_S)) {
      move_dir = 0;
    }
    else if (IsKeyDown(KEY_A) && IsKeyDown(KEY_S)) {
      if (move_dir == 1) player.facing = -1;
      else player.facing = 1;
    }
    else if (IsKeyDown(KEY_A)) {
      move_dir = -1;
      if (player.facing != -1) player.facing = -1;
    }
    else if (IsKeyDown(KEY_S)) {
      move_dir = 1;
      if (player.facing != 1) player.facing = 1;
    }
    if (IsKeyDown(KEY_R)) player.anim = SPRITE_PLAYER_CROUCH;
    else player.anim = SPRITE_PLAYER;
    // if (IsKeyDown(KEY_SPACE))

    jumping = false;
    if (IsKeyDown(KEY_W)) jumping = true;

    // :update
    if (move_dir == 1) player.position.x += run_speed * delta_t;
    if (move_dir == -1) player.position.x -= run_speed * delta_t;

    // :jump :gravity
    if (jumping && jump_state == JUMP_AVAILABLE || jump_state == JUMP_ASCENDING) {
      player.position.y -= 400 * delta_t;
      jump_count += 1;
      if (jump_count >= 8) jump_state = JUMP_DESCENDING;
    }
    else if (player.position.y < 100) {
      player.position.y += 100 * delta_t;
    }
    else {
      jump_state = JUMP_AVAILABLE;
      jump_count = 0;
    }
    camera.target.x = player.position.x;
    camera.target.y = player.position.y;

    player.sprite = &sprite[player.anim];

    BeginDrawing();
    ClearBackground(COLOR_BACKGROUND);
    BeginMode2D(camera);
    DrawLine(100, 0, 100, window_height, RAYWHITE);
    DrawRectangle(0, 114, window_width, window_height/2, COLOR_EARTH);
    DrawLine(0, window_height/2, window_width, window_height/2, RAYWHITE);

    for (int i = 0; i < 30; i++) {
      DrawTexture(*en[i].sprite, en[i].position.x, en[i].position.y, en[i].color);
    }
    // :draw player
    DrawTexturePro(
    *player.sprite,
    (Rectangle) {0, 0, 16 * player.facing, 16},
    (Rectangle) {player.position.x, player.position.y, 16, 16},
    (Vector2) {0, 0},
    0,
    RAYWHITE);

    EndMode2D();

    if (ui_open) {
      DrawRectangle(0, 0, window_width/4, window_height, LIGHTGRAY);
      DrawText(TextFormat("%i", ui_open), 10, 200, 20, BLACK);
      // :ui draw
    }

    if (debug) {
      DrawText("Debugging", 10, 10, 20, RAYWHITE);
      DrawText(TextFormat("Player X: %f", player.position.x), 10, 30, 20, RAYWHITE);
      DrawText(TextFormat("Player Y: %f", player.position.y), 10, 50, 20, RAYWHITE);
      DrawText(TextFormat("Delta T: %f", delta_t), 10, 80, 20, RAYWHITE);
      // DrawText(TextFormat("count: %i", counter), 10, 80, 20, RAYWHITE);
      // :debug draw
    }
    EndDrawing();

  }
}
