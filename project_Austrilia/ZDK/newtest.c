#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <ncurses.h>


//Configuration
#define DELAY (10)
#define AVATAR_WIDTH (4)
#define AVATAR_HEIGHT (3)
#define EXIT_DOOR_WIDTH (6)
#define EXIT_DOOR_HEIGHT (5)
#define MONSTER_WIDTH (5)
#define MONSTER_HEIGHT (5)
//walking speed should be calibrated so that it takes ca. 5 sec from left to right
#define WALKING_SPEED (1)
#define RUNNING_SPEED (2)
//roughly 5 or 6 times its own height, overall time is around 1.5-2.0 seconds
#define INITIAL_VERTICAL_SPEED (-0.5)
#define GRAVITATIONAL_ACCELERATION (0.5)


float speed_x = 0;
float speed_y = 0;

int avatar_initial_position_y = 0;
int avatar_initial_position_x = 0;


//Game state
bool game_over = false;
bool update_screen = true;


//Status display
time_t minutes = 0;
time_t seconds = 0;
time_t delay_count = 0;
time_t new_level_time = 0;
time_t million_seconds = 0;
time_t jump_delay_count = 0;
bool timer_on = false;
int lives = 10;
int level = 1;
int score = 0;





char* avatar_image =
" O  "
"/|\\ "
"L L " ;


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

char* debug_msg_image_a =
"this is A message";
char* debug_msg_image_b =
"this is B message";




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

void debug_message_a(){
  int w = screen_width(), h = screen_height(), ch = '*';
  clear_screen();
  int message_width = strlen(debug_msg_image_a) / 2;
  sprite_id debug_msg = sprite_create( ( w - message_width ) / 2, ( h - 2 ) / 2, message_width, 2, debug_msg_image_a);
  sprite_draw(debug_msg);
  show_screen();
  wait_char();
  return;
}

void debug_message_b(){
  int w = screen_width(), h = screen_height(), ch = '*';
  clear_screen();
  int message_width = strlen(debug_msg_image_b) / 2;
  sprite_id debug_msg = sprite_create( ( w - message_width ) / 2, ( h - 2 ) / 2, message_width, 2, debug_msg_image_b);
  sprite_draw(debug_msg);
  show_screen();
  wait_char();
  return;
}

void setup(void){

  draw_line(0,2,screen_width()-1,2,'-');
  draw_line(0,screen_height()-1,screen_width()-1,screen_height()-1,'=');
  draw_line(0,0,0,screen_height()-1,'|');
  draw_line(screen_width()-1,0,screen_width()-1,screen_height()-1,'|');
  show_screen();


// set up  avatar's position
// standing on the floor of the cavern near the left wall
int aw = AVATAR_WIDTH, ah = AVATAR_HEIGHT;
int ax = aw/2+1;
int ay = (screen_height()-ah-1);
avatar_initial_position_y = ay;
avatar_initial_position_x = ax;
avatar = sprite_create(ax,ay,aw,ah,avatar_image);

sprite_draw(avatar);
show_screen();


//set up exit_door's position, this is level1's position
//
int ew = EXIT_DOOR_WIDTH, eh = EXIT_DOOR_HEIGHT;
int ex = (screen_width()-ew-1);
int ey = (screen_height()-eh-1);
exit_door = sprite_create(ex,ey,ew,eh,exit_door_image);
sprite_draw(exit_door);
show_screen();

//set up monster
int mw = MONSTER_WIDTH, mh = MONSTER_HEIGHT;
int mx = (screen_width()-mw-1);
int my = (screen_height()-mh-1);
monster = sprite_create(mx,my,mw,mh,monster_image);
sprite_draw(monster);
show_screen();

int width = screen_width() / 4;
draw_formatted(2, 1, "* Time: %02d:%02d", minutes, seconds);
draw_formatted(width, 1, "* lives: %d", lives);
draw_formatted(width * 2, 1, "* level: %d", level);
draw_formatted(width *3, 1, "* score: %d", score);
show_screen();

}


void display_status(){
  int width = screen_width() / 4;
	draw_formatted(2, 1, "* Time: %02d:%02d", minutes, seconds);
	draw_formatted(width, 1, "* lives: %d", lives);
	draw_formatted(width * 2, 1, "* level: %d", level);
	draw_formatted(width *3, 1, "* score: %d", score);
  draw_formatted(width *3, 15, "* jump_delay_count/100: %d", jump_delay_count/100);
  show_screen();
}

void display_help(){
  int w = screen_width()/2;
  int h = screen_height()/2;
  draw_string(w,h,"arrow key for movemeng, 'l' for next level, 'q' for quit");
  show_screen();
}

void clock() {
	delay_count++;
	if (delay_count == 100) {
		seconds++;
		new_level_time++;
		delay_count = 0;
		if (seconds == 60) {
			seconds = 0;
			minutes++;
		}
	}
}

void jump_timer(){
  if(timer_on) jump_delay_count++;
  else return;
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
  int w = screen_width(), h = screen_height(), ch = '*';

	// (f) Get a character code from standard input without waiting.
	int key = getch();

	// (y) Test for end of game.
	if ( key == 'q' ) {
		clear_screen();
		int message_width = strlen(msg_image) / 2;
		sprite_id msg = sprite_create( ( w - message_width ) / 2, ( h - 2 ) / 2, message_width, 2, msg_image);
		sprite_draw(msg);
		show_screen();
		game_over = true;
		wait_char();
		return;
    //debug_message();
	}

  //current position of avatar
  int ax = round(sprite_x(avatar));
  int ay = round(sprite_y(avatar));

  int next_position_x = ax + speed_x;
  int next_position_y = ay + speed_y;

  bool out_right_bound = (next_position_x > w-1);
  bool out_left_bound = (next_position_x < 0);


  //in mid-air
  //none of keys have any effect
  //5 cases, touches monster, exit, floor_bottom, platform_bottom, platform_top
  //but now only consider floor_bottom
  bool out_floor_bound = (next_position_y > (h-sprite_height(avatar)/2-1));
  bool out_ceiling_bound = (next_position_y < sprite_height(avatar)/2+3);

  if (out_left_bound||out_right_bound) {
    if (out_left_bound){
      speed_x = 0;
      sprite_move_to (avatar,1,avatar_initial_position_y);
    }
    if (out_right_bound){
      speed_x = 0;
      sprite_move_to (avatar,w-6,avatar_initial_position_y);
    }
  }
  else{
    //is on the floor
    if (ay == avatar_initial_position_y) {
      // up_key event is independent
      if (key =='w') {
        timer_on = true;
        speed_y = INITIAL_VERTICAL_SPEED;
      }

      //not moving horizontally
      if (speed_x == 0) {
        //running towards left
        if ( key == 'a' && ax > 1 ) speed_x = -WALKING_SPEED;
        //walking towards right
        if ( key == 'd' && ax < w - sprite_width(avatar) - 1 ) speed_x = WALKING_SPEED;
      }
      //moving horizontally
      else{
        //walking towards right at walking speed
        if(speed_x == WALKING_SPEED){
          if ( key == 'a' && ax > 1 ) speed_x = 0;
          if ( key == 'd' && ax < w - sprite_width(avatar) - 1 ) speed_x = RUNNING_SPEED;
        }
        //walking towards left at walking speed
        if(speed_x == -WALKING_SPEED){
          if ( key == 'a' && ax > 1 ) speed_x = -RUNNING_SPEED;
          if ( key == 'd' && ax < w - sprite_width(avatar) - 1 ) speed_x = 0;
        }
        //running towards right at running speed
        if(speed_x == RUNNING_SPEED){
          if ( key == 'a' && ax > 1 ) speed_x = WALKING_SPEED;
        }
        //running towards left at running speed
        if(speed_x == -RUNNING_SPEED){
          if ( key == 'd' && ax < w - sprite_width(avatar) - 1 ) speed_x = -WALKING_SPEED;
        }
      }
    }
    //is in mid-air
    else{
      //none of keys have any effect
      //5 cases, touches monster, exit, floor_bottom, platform_bottom, platform_top
      //but now only consider floor_bottom
      if ( out_floor_bound ) {
        speed_y = 0;
        sprite_move_to (avatar,sprite_x(avatar),avatar_initial_position_y);
        timer_on = false;
        jump_delay_count = 0;
      }
      if (out_ceiling_bound) {
        sprite_move(avatar,0,1);
        jump_delay_count = 0;
        speed_y = 0;
        debug_message_b();

      }

      //otherwise,normal situation
      else{
        speed_y += GRAVITATIONAL_ACCELERATION * jump_delay_count/100;
      }

    }

  }

  sprite_move(avatar,speed_x,speed_y);
  // Leave next line intact
	clear_screen();

	// (b) Draw the border (process).
  draw_line(0,2,screen_width()-1,2,'-');
  draw_line(0,screen_height()-1,screen_width()-1,screen_height()-1,'=');
  draw_line(0,0,0,screen_height()-1,'|');
  draw_line(screen_width()-1,0,screen_width()-1,screen_height()-1,'|');

  display_status();

	// (l)	Draw the hero.
	sprite_draw( avatar );

}



void cleanup(void){

}

int main(void){
  WINDOW *menu_win;
  keypad(menu_win, TRUE);

  setup_screen();
  setup();
  show_screen();

  while (!game_over) {
    clock();
    jump_timer();


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
