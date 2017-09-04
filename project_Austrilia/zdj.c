#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// ADD INCLUDES HERE

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define HERO_WIDTH (5)
#define HERO_HEIGHT (5)
#define ZOMBIE_WIDTH (5)
#define ZOMBIE_HEIGHT (5)

// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */

char * hero_image =
/**/	"H   H"
/**/	"H   H"
/**/	"HHHHH"
/**/	"H   H"
/**/	"H   H";

char * zombie_image =
/**/	"ZZZZZ"
/**/	"   Z "
/**/	"  Z  "
/**/	" Z   "
/**/	"ZZZZZ";

char * msg_image =
/**/	"Goodbye and thank-you for playing ZombieDash Jr."
/**/	"            Press any key to exit...            ";

// (c) Declare a sprite_id called hero.
sprite_id hero;

// (m) Declare a sprite_id called zombie.
sprite_id zombie;

// Setup game.
void setup(void) {
	// (a) Draw the border (setup).
	int w = screen_width(), h = screen_height(), ch = '*';
	draw_line(0,0,0,h-1,ch);
	draw_line(0,0,w-1,0,ch);
	draw_line(0,h-1,w-1,h-1,ch);
	draw_line(w-1,0,w-1,h-1,ch);

	// Keep the next line intact.
	show_screen();

	// (d) Set up the hero at the centre of the screen.
	int hw = HERO_WIDTH, hh = HERO_HEIGHT;
	int x = (w-hw)/2, y = (h - hh) / 2;
	hero = sprite_create(x, y, hw, hh, hero_image);

	// (e) Draw the hero.
	sprite_draw( hero );

	// Keep the next line intact.
	show_screen();

	// (n)	Set up the zombie at a random location on the screen.
	//srand(get_current_time());
	srand( 9139532 );
	int zx = 1 + rand() % ( w - ZOMBIE_WIDTH - 2 );
	int zy = 1 + rand() % ( h - ZOMBIE_HEIGHT - 2 );
	zombie = sprite_create( zx, zy, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, zombie_image );

	// (o) Draw the zombie.
	sprite_draw(zombie);

	// (p) Set the zombie in motion.
	int angle = rand() % 360;
	sprite_turn_to( zombie, 0.1, 0 );
	sprite_turn( zombie, angle );

	// Keep the next line intact.
	show_screen();
}

bool collision() {
	bool collided = true;
	
	int hx = round(sprite_x(hero));
	int hy = round(sprite_y(hero));
	int hr = hx + HERO_WIDTH - 1;
	int hb = hy + HERO_HEIGHT - 1;
	
	int zx = round(sprite_x(zombie));
	int zy = round(sprite_y(zombie));
	int zr = zx + ZOMBIE_WIDTH - 1;
	int zb = zy + ZOMBIE_HEIGHT - 1;
	
	if (hr<zx) {collided = false;}
	if (hb<zy) {collided = false;}
	
	if (zr<hx) {collided = false;}
	if (zb<hy) {collided = false;}
	
	return collided;
}

// Play one turn of game.
void process(void) {
	int w = screen_width(), h = screen_height(), ch = '*';

	// (f) Get a character code from standard input without waiting.
	int key = get_char();
	
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
	}

	// (g)	Get the current screen coordinates of the hero in integer variables 
	//		by rounding the actual coordinates.
	int hx = round(sprite_x(hero));
	int hy = round(sprite_y(hero));

	// (h) Move hero left according to specification.
	if ( key == 'a' && hx > 1 ) sprite_move( hero, -1, 0 );

	// (i) Move hero right according to specification.
	if ( key == 'd' && hx < w - sprite_width(hero) - 1 ) sprite_move( hero, +1, 0 );

	// (j) Move hero up according to specification.
	if ( key == 'w' && hy > 1 ) sprite_move( hero, 0, -1 );

	// (k) Move hero down according to specification.
	if ( key == 's' && hy < h - sprite_height(hero) - 1 ) sprite_move( hero, 0, +1 );

	// (q.a) Test to move the zombie if key is 'z' or negative.
	if ( key == 'z' /*|| key < 0*/ ) {
		// (r) Zombie takes one step.
		sprite_step( zombie );

		// (s) Get screen location of zombie.
		int zx = round(sprite_x(zombie));
		int zy = round(sprite_y(zombie));

		// (t) Get the displacement vector of the zombie.
		double zdx = sprite_dx( zombie );
		double zdy = sprite_dy( zombie );

		// (u) Test to see if the zombie hit the left or right border.
		if ( zx <= 0 ) {
			zdx = fabs(zdx);
		} 
		else if ( zx >= w - ZOMBIE_WIDTH ) {
			zdx = -fabs(zdx);
		}

		// (v) Test to see if the zombie hit the top or bottom border.
		if ( zy <= 0 ) {
			zdy = fabs(zdy);
		} 
		else if ( zy >= h - ZOMBIE_HEIGHT ) {
			zdy = -fabs(zdy);
		}

		// (w) Test to see if the zombie needs to step back and change direction.
		if ( zdx != sprite_dx( zombie ) || zdy != sprite_dy(zombie) ) {
			sprite_back( zombie );
			sprite_turn_to( zombie, zdx, zdy );
		}

	// (q.b) End else-if test to move the zombie if key is 'z' or negative.
	}
	
	if ( collision()) {
		clear_screen();
		int message_width = strlen(msg_image) / 2;
		sprite_id msg = sprite_create( ( w - message_width ) / 2, ( h - 2 ) / 2, message_width, 2, msg_image);
		sprite_draw(msg);
		show_screen();
		game_over = true;
		wait_char();
		return;
	}

	// Leave next line intact
	clear_screen();

	// (b) Draw the border (process).
	draw_line(0,0,0,h-1,ch);
	draw_line(0,0,w-1,0,ch);
	draw_line(0,h-1,w-1,h-1,ch);
	draw_line(w-1,0,w-1,h-1,ch);

	// (l)	Draw the hero.
	sprite_draw( hero );

	// (x)	Draw the zombie.
	sprite_draw( zombie ); 
}

// Clean up game
void cleanup(void) {
	// STATEMENTS
}

// Program entry point.
int main(void) {
	setup_screen();
	setup();
	show_screen();

	while ( !game_over ) {
		process();

		if ( update_screen ) {
			show_screen();
		}

		timer_pause(DELAY);
	}

	cleanup();

	return 0;
}