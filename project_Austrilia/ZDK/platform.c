#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

//Configuration
#define DELAY (10)
#define AVATAR_WIDTH (5)
#define AVATAR_HEIGHT (5)
//walking speed should be calibrated so that it takes ca. 5 sec from left to right
#define WALKING_SPEED (2)
#define RUNNING_SPEED (4)


//Game state
bool game_over = false;
bool update_screen = true;
bool display_dialog = false;
//bool is_idle = true;
bool moving_horizontally = false;
bool is_running = false;
bool facing_right = false;
bool is_air = false;

//Status display
time_t minutes = 0;
time_t seconds = 0;
int lives = 10;
int level = 1;
int score = 0;




char* avatar_image =
"┌(^o^)┐"
"  | |   ";





// Declare sprite
sprite_id avatar;
//no sprite of floor, decide whether on the floor by testing avatar's height
// sprite_id floor;
//and thus platform_spirte may be discarded as well
sprite_id platform_level1;
sprite_id monster;
sprite_id treasure;
sprite_id exit;
sprite_id key;

void setup(void){
// draw the border

// status top line
draw_line(0,2,screen_width()-1,2,'-');
// // status bottom line
// draw_line(0,2,screen_width()-1,2,'-');
// bottom line
draw_line(0,screen_height()-1,screen_width()-1,screen_height()-1,'=');
// left line
draw_line(0,0,0,screen_height()-1,'|');
// right line
draw_line(screen_width()-1,0,screen_width()-1,screen_height()-1,'|');

// set up the avatar's position
// standing on the floor of the cavern near the left wall
int aw = AVATAR_WIDTH, ah = AVATAR_HEIGHT;
int ax = aw+1;
int ay = (screen_height()-ah-1);
avatar = sprite_create(ax,ay,aw,ah,avatar_image);

//draw the avatar
sprite_draw(avatar);

}

bool is_collided(sprite_id s1, sprite_id s2){
  bool collided = true;

  int s1_top = round(sprite_y(s1));
  int s1_bottom = s1_top + sprite_height(s1);
  int s1_left = round(sprite_x(s1));
  int s1_right = s1_left + sprite_width(s1);

  int s2_top = round(sprite_y(s2));
  int s2_bottom = s2_top + sprite_height(s2);
  int s2_left = round(sprite_x(s2));
  int s2_right = s2_left + sprite_width(s2);

  if (s1_right<s2_left) {collided = false;}
  if (s1_bottom<s2_top) {collided = false;}
  if (s1_top>s2_bottom) {collided = false;}
  if (s1_left>s2_left) {collided = false;}

  return collided;
}


void process(){
  int w = screen_width();
  int h = screen_height();


  int initial_avatar_y = h - AVATAR_HEIGHT -1;

  int current_avatar_x = round(spirte_x(avatar));
  int current_avatar_y = round(spirte_y(avatar));

  float speed = 0;


  int key = get_ch();

  //on the floor, redungdant codes...
  if (current_avatar_y == initial_avatar_y) {
    if (!moving_horizontally) {
      if (key == KEY_RIGHT) {
        moving_horizontally = true;
        facing_right = true;
        speed = WALKING_SPEED;
      }
      if (key == KEY_LEFT) {
        moving_horizontally = true;
        facing_right = false;
        speed = -WALKING_SPEED;
      }
    }

    //If the avatar is moving towards the right at walking speed
    if (facing_right==true && sprite_dx(avatar)==abs(WALKING_SPEED)) {
      if (key == KEY_RIGHT) {  speed = RUNNING_SPEED;}
      if (key == KEY_LEFT) {speed = 0;  moving_horizontally = false; }
    }
    //If the avatar is moving towards the right at running speed
    if (facing_right==true && sprite_dx(avatar)==abs(RUNNING_SPEED)) {
      if (key == KEY_LEFT) {speed = WALKING_SPEED;}
    }

    //If the avatar is moving towards the left at walking speed
    if (facing_right==false&& sprite_dx(avatar)==abs(WALKING_SPEED)) {
      if (key == KEY_RIGHT) {speed =0; moving_horizontally = false;}
      if (key == KEY_LEFT) {speed = RUNNING_SPEED;}
    }
    //If the avatar is moving towards the left at running speed
    if (facing_right==false&& sprite_dx(avatar)==abs(RUNNING_SPEED)) {
      if (key == KEY_RIGHT) {speed = WALKING_SPEED;}
    }

    bool will_continue = (!is_collided(avatar,monster))||(!is_collided(avatar,wall))||(is_collided(avatar,exit));
    while(will_continue){sprite_step(avatar);}
  }
}
