/*

   "Classic" Digital Watch Pebble App

 */

// Standard includes
#include "pebble.h"


// App-specific data
Window *window; // All apps must have at least one window
TextLayer *title_layer; // "It's 5 o'clock in"
TextLayer *city_layer; // City name
char *cities_text[24] = // Array of 24 cities, one for each hour. Each city name is 20 characters long.
{
	"Majuro", 			// 0
	"Sydney", 			// 1
	"Brisbane", 		// 2
	"Tokyo", 			// 3
	"Hong Kong", 		// 4
	"Bangkok", 			// 5
	"Dhaka", 			// 6
	"Karachi", 			// 7
	"Moscow",			// 8
	"Baghdad",			// 9
	"Cairo",			// 10
	"Madrid",			// 11
	"London",			// 12
	"Praia",			// 13
	"Rio de Janeiro",	// 14
	"Buenos Aires",		// 15
	"Santo Domingo",	// 16
	"New York",			// 17
	"New Orleans",		// 18
	"Phoenix",			// 19
	"Tijuana",			// 20
	"Anchorage",		// 21
	"Honolulu",			// 22
	"Pago Pago"			// 23
};

// Called once per second
static void handle_hour_tick(struct tm* tick_time, TimeUnits units_changed)
{
	char time_text[20];
	clock_copy_time_string(time_text, 20);
	static char out_text[40];
	out_text[0] = '\0';
	strcat(out_text, cities_text[tick_time->tm_hour]);
	strcat(out_text, "\n");
	strcat(out_text, time_text);
	text_layer_set_text(city_layer, out_text);
}

// Handle the start-up of the app
static void do_init(void) {

  // Create our app's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);
	
  // Init the text layer used to show the time
  title_layer = text_layer_create(GRect(29, 24, 144-40 /* width */, 168-54 /* height */));
  text_layer_set_text_color(title_layer, GColorWhite);
  text_layer_set_background_color(title_layer, GColorClear);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(title_layer, "It's 5 o'clock in");

  // Init the text layer used to show the time
  city_layer = text_layer_create(GRect(29, 84, 144-40 /* width */, 168-54 /* height */));
  text_layer_set_text_color(city_layer, GColorWhite);
  text_layer_set_background_color(city_layer, GColorClear);
  text_layer_set_font(city_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_hour_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, &handle_hour_tick);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(title_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(city_layer));
}

static void do_deinit(void)
{
  text_layer_destroy(title_layer);
  text_layer_destroy(city_layer);
  window_destroy(window);
}

// The main event/run loop for our app
int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
