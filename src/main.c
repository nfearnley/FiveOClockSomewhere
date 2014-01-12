// Standard includes
#include <pebble.h>
#include "main.h"

// App-specific data
Window *window; // All apps must have at least one window
TextLayer *city_layer; // City name
char city_name[20];
int city_hour = -1;

// Load the current city from persistant memory
static void load_city(int hour)
{
    if (persist_exists(hour))
    {
        persist_read_data(hour, &city_name, sizeof(city_name));
        city_hour = hour;
    }
    else
    {
        strcpy(city_name, "Somewhere");
    }
}

// Save the current city to persistant memory
static void save_city()
{
    persist_write_data(city_hour, &city_name, sizeof(city_name));
}

static void download_city(int hour)
{
    
}

// Get the time text
static void get_text(char *out_text, struct tm *tick_time)
{
    int hour = tick_time->tm_hour;
    if (city_hour != hour)
    {
        load_city(hour);
    }
    strftime(out_text, 14, "It's 5:%M in\n", tick_time); // 14 bytes
    strcat(out_text, city_name); // <=20 bytes
}

// Called once per minute
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed)
{
    static char out_text[38];
    get_text(out_text, tick_time);
    text_layer_set_text(city_layer, out_text);
}

// Handle the start-up of the app
static void do_init(void)
{
    // Create our app's base window
    window = window_create();
    window_stack_push(window, true);
    window_set_background_color(window, GColorBlack);

    // Init the text layer used to show the time
    city_layer = text_layer_create(GRect(0, 0, 144, 168));
    text_layer_set_text_color(city_layer, GColorWhite);
    text_layer_set_background_color(city_layer, GColorClear);
    text_layer_set_font(city_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    
    // Add text layer to window root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(city_layer));

    // Ensures time is displayed immediately (will break if NULL tick event accessed).
    // (This is why it's a good idea to have a separate routine to do the update itself.)
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    handle_minute_tick(current_time, MINUTE_UNIT);
    
    // Subscribe minute handler
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

}

// Handle the shutdown of the app
static void do_deinit(void)
{
    text_layer_destroy(city_layer);
    window_destroy(window);
}

// The main event/run loop for our app
int main(void)
{
    do_init();
    app_event_loop();
    do_deinit();
}
