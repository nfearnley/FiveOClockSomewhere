// Standard includes
#include <pebble.h>
#include "PDUtils.h"
// The main event/run loop for our app

struct city
{
    char name[20];
    struct tm expiry;
};

// App-specific data
Window *window; // All apps must have at least one window
TextLayer *city_layer; // City name
struct city cities[24] = // Array of 24 cities, one for each hour. Each city name is up to 20 characters long.
{
    //"Majuro",           // 0
    //"Sydney",           // 1
    //"Brisbane",         // 2
    //"Tokyo",            // 3
    //"Hong Kong",        // 4
    //"Bangkok",          // 5
    //"Dhaka",            // 6
    //"Karachi",          // 7
    //"Moscow",           // 8
    //"Baghdad",          // 9
    //"Cairo",            // 10
    //"Madrid",           // 11
    //"London",           // 12
    //"Praia",            // 13
    //"Rio de Janeiro",   // 14
    //"Buenos Aires",     // 15
    //"Santo Domingo",    // 16
    //"New York",         // 17
    //"New Orleans",      // 18
    //"Phoenix",          // 19
    //"Tijuana",          // 20
    //"Anchorage",        // 21
    //"Honolulu",         // 22
    //"Pago Pago"         // 23
    {"Somewhere", {0}},   // 0
    {"Somewhere", {0}},   // 1
    {"Somewhere", {0}},   // 2
    {"Somewhere", {0}},   // 3
    {"Somewhere", {0}},   // 4
    {"Somewhere", {0}},   // 5
    {"Somewhere", {0}},   // 6
    {"Somewhere", {0}},   // 7
    {"Somewhere", {0}},   // 8
    {"Somewhere", {0}},   // 9
    {"Somewhere", {0}},   // 10
    {"Somewhere", {0}},   // 11
    {"Somewhere", {0}},   // 12
    {"Somewhere", {0}},   // 13
    {"Somewhere", {0}},   // 14
    {"Somewhere", {0}},   // 15
    {"Somewhere", {0}},   // 16
    {"Somewhere", {0}},   // 17
    {"Somewhere", {0}},   // 18
    {"Somewhere", {0}},   // 19
    {"Somewhere", {0}},   // 20
    {"Somewhere", {0}},   // 21
    {"Somewhere", {0}},   // 22
    {"Somewhere", {0}}    // 23
};

static void load_city(int hour)
{
    if (persist_exists(hour))
    {
        app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Loading: %i", hour);
        //persist_read_data(hour, &cities[hour], sizeof(cities[hour]));
    }
}

static void save_city(int hour)
{
    app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Saving: %i", hour);
    //persist_write_data(hour, &(cities[hour]), sizeof(cities[hour]));
}

static void get_city(struct tm* tick_time)
{
    struct tm *tm_expiry = &(cities[tick_time->tm_hour].expiry);
    time_t time_expiry = p_mktime(tm_expiry);
    //time_t time_now = mktime(tick_time);
    //double diff = difftime(time_now, time_expiry);
    //app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Diff: %ld", (long)diff);
    //if (diff < 0)
    //{
    //    
    //}
}

static void get_text(char *out_text, struct tm* tick_time)
{
    strftime(out_text, 14, "It's 5:%M in\n", tick_time); // 14 bytes
    strcat(out_text, (cities[tick_time->tm_hour]).name); // <=20 bytes
}

// Called once per second
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed)
{
    static char out_text[38]; // <=38 bytes
    get_text(out_text, tick_time);
    text_layer_set_text(city_layer, out_text);
    get_city(tick_time);
}

// Handle the start-up of the app
static void do_init(void)
{
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    
    // Create our app's base window
    window = window_create();
    window_stack_push(window, true);
    window_set_background_color(window, GColorBlack);

    // Init the text layer used to show the time
    city_layer = text_layer_create(GRect(0, 0, 144, 168));
    text_layer_set_text_color(city_layer, GColorWhite);
    text_layer_set_background_color(city_layer, GColorClear);
    text_layer_set_font(city_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

    // Ensures time is displayed immediately (will break if NULL tick event accessed).
    // (This is why it's a good idea to have a separate routine to do the update itself.)
    handle_minute_tick(current_time, MINUTE_UNIT);
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(city_layer));
}

static void do_deinit(void)
{
    save_city(0);
    
    text_layer_destroy(city_layer);
    window_destroy(window);
}

int main(void)
{
    //do_init();
    app_event_loop();
    //do_deinit();
}
