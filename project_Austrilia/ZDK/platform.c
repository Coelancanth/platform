#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <ncurses.h>


//Configuration
#define DELAY (10)
#define AVATAR_WIDTH (5)
#define AVATAR_HEIGHT (5)
#define EXIT_DOOR_WIDTH (6)
#define EXIT_DOOR_HEIGHT (5)
#define MONSTER_WIDTH (5)
#define MONSTER_HEIGHT (5)
//walking speed should be calibrated so that it takes ca. 5 sec from left to right
#define WALKING_SPEED (2)
#define RUNNING_SPEED (4)
//roughly 5 or 6 times its own height, overall time is around 1.5-2.0 seconds
#define INITIAL_VERTICAL_SPEED()
#define GRAVITATIONAL_ACCELERATION()


//Game state
bool game_over = false;
bool update_screen = true;
bool display_status_flag = false;
bool new_game = false;
//bool is_idle = true;
bool moving_horizontally = false;
bool is_running = false;
bool is_air = false;

//Status display
time_t minutes = 0;
time_t seconds = 0;
int lives = 10;
int level = 1;
int score = 0;

double speed_x = 0;
double speed_y = 0;




char* avatar_image =
/**/	"  O  "
/**/	"/ |\\ "
/**/	"  |  "
/**/	" / \\ "
/**/	"/   \\";


char* msg_image =
/**/	"Goodbye and thank-you for playing "
/**/	"    Press any key to exit...      ";

char* exit_door_image =
" EXIT "
"|    |"
"|   -|"
"|    |"
"|    |";

char* monster_image =
// "___O   "
// "   |   "
// "   |   "
// "  / \\ "
// " /   \\";
/**/	"ZZZZZ"
/**/	"   Z "
/**/	"  Z  "
/**/	" Z   "
/**/	"ZZZZZ";




// Declare sprite
sprite_id avatar;
//no sprite of floor, decide whether on the floor by testing avatar's height
// sprite_id floor;
//and thus platform_spirte may be discarded as well
sprite_id platform_level1;
sprite_id monster;
sprite_id treasure;
sprite_id exit_door;
sprite_id key;

void setup(void){

// set up  avatar's position
// standing on the floor of the cavern near the left wall
int aw = AVATAR_WIDTH, ah = AVATAR_HEIGHT;
int ax = aw/2+1;
int ay = (screen_height()-ah-1);
avatar = sprite_create(ax,ay,aw,ah,avatar_image);

//set up exit_door's position, this is level1's position
//
int ew = EXIT_DOOR_WIDTH, eh = EXIT_DOOR_HEIGHT;
int ex = (screen_width()-ew-1);
int ey = (screen_height()-eh-1);
exit_door = sprite_create(ex,ey,ew,eh,exit_door_image);

//set up monster
int mw = MONSTER_WIDTH, mh = MONSTER_HEIGHT;
int mx = (screen_width()-mw-1);
int my = (screen_height()-mh-1);
monster = sprite_create(mx,my,mw,mh,monster_image);
}



void draw_border(){
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
}


void display_status(){
  int width = screen_width() / 4;
	draw_formatted(2, 1, "* Time: %02d:%02d", minutes, seconds);
	draw_formatted(width, 1, "* lives: %d", lives);
	draw_formatted(width * 2, 1, "* level: %d", level);
	draw_formatted(width *3, 1, "* score: %d", score);
}

void display_help(){
  int w = screen_width()/2;
  int h = screen_height()/2;
  draw_string(w,h,"arrow key for movemeng, 'l' for next level, 'q' for quit");
  show_screen();
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

  int current_avatar_x = round(sprite_x(avatar));
  int current_avatar_y = round(sprite_y(avatar));

  float speed_x = 0;
  float speed_y = 0;

  int key1 = get_char();
  int key = getch();


  if ( key1 == 'a' ) {
		clear_screen();
		int message_width = strlen(msg_image) / 2;
		sprite_id msg = sprite_create( ( w - message_width ) / 2, ( h - 2 ) / 2, message_width, 2, msg_image);
		sprite_draw(msg);
		show_screen();
		game_over = true;
		wait_char();
		return;
	}










  bool will_continue = (!is_collided(avatar,monster))||(is_collided(avatar,exit_door));
  while(will_continue){

    //on the floor
    if (current_avatar_y == initial_avatar_y) {
      //up-arrow key will only affect vertical changes
      if (key == KEY_UP) {

        //@TODO:
        //calculate elapsed_time

      //   speed_y = INITIAL_VERTICAL_SPEED-GRAVITATIONAL_ACCELERATION * (elapsed_time)
      }

      //return ture if passes the right end wall
      bool right_end = (current_avatar_x>=w-sprite_width(avatar)-1);
      bool left_end =  (current_avatar_x<=1);
      bool top_end = (current_avatar_y<=1);
      bool bottom_end = (current_avatar_y>= h-sprite_height(avatar)-1);


      if (!moving_horizontally) {
        if (key == KEY_RIGHT&&(!right_end)) {
          moving_horizontally = true;
          speed_x = WALKING_SPEED;
        }
        if (key == KEY_LEFT&&(!left_end)) {
          moving_horizontally = true;
          speed_x = -WALKING_SPEED;
        }
      }

      //If the avatar is moving towards the right at walking speed
      if (speed_x == WALKING_SPEED && (!right_end)) {
        if (key == KEY_RIGHT) {  speed_x = RUNNING_SPEED;}
        if (key == KEY_LEFT) {speed_x = 0;  moving_horizontally = false; }
      }
      //If the avatar is moving towards the right at running speed
      if (speed_x == RUNNING_SPEED && (!right_end)) {
        if (key == KEY_LEFT) {speed_x = WALKING_SPEED;}
      }

      //If the avatar is moving towards the left at walking speed
      if (speed_x == -WALKING_SPEED && (!left_end)) {
        if (key == KEY_RIGHT) {speed_x =0; moving_horizontally = false;}
        if (key == KEY_LEFT) {speed_x = RUNNING_SPEED;}
      }
      //If the avatar is moving towards the left at running speed
      if (speed_x == -RUNNING_SPEED &&(!left_end)) {
        if (key == KEY_RIGHT) {speed_x = WALKING_SPEED;}
      }
    }

    //in the mid-air
    else{


    }






    sprite_move(avatar,speed_x,speed_y);
  }

    if (is_collided(avatar,monster)) {
      clear_screen();
		  int message_width = strlen(msg_image) / 2;
		  sprite_id msg = sprite_create( ( w - message_width ) / 2, ( h - 2 ) / 2, message_width, 2, msg_image);
		  sprite_draw(msg);
		  show_screen();
		  game_over = true;
		  wait_char();
		  return;
    }

    if (is_collided(avatar,exit_door)) {
      /* code */
    }

    // clear_screen();
    //
    // draw_line(0,2,screen_width()-1,2,'-');
    // draw_line(0,screen_height()-1,screen_width()-1,screen_height()-1,'=');
    // draw_line(0,0,0,screen_height()-1,'|');
    // draw_line(screen_width()-1,0,screen_width()-1,screen_height()-1,'|');

    sprite_draw(monster);
    sprite_draw(avatar);
    sprite_draw(exit_door);
}

void cleanup(void){

}

int main(void){
  setup_screen();
  setup();
  show_screen();

  while (!game_over) {
    clear_screen();
    // display_status();
    // draw_border();


    show_screen();
    process();

    if (update_screen) {
      show_screen();
    }
    timer_pause(DELAY);
  }

  cleanup();
  return 0;
}
