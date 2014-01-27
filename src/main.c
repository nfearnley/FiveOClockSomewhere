// Standard includes
#include <pebble.h>
#include "main.h"

// App-specific data
Window *window; // All apps must have at least one window
TextLayer *city_layer; // City name

char city_name[20];
int city_hour = -1;

enum
{
    CITY_KEY_HOUR = 0x0,
    CITY_KEY_NAME = 0x1
};

enum RequestStatus
{
    REQ_NOT_SENT,
    REQ_SENT,
    RESP_RECEIVED
};

enum RequestStatus req_status = REQ_NOT_SENT;

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
        strncpy(city_name, "Somewhere", sizeof(city_name));
    }
}

// Save the current city to persistant memory
static void save_city(int hour, char *name, int name_size)
{
    persist_write_data(hour, &name, name_size);
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
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
    int hour = tick_time->tm_hour;
    static char out_text[38];
    get_text(out_text, tick_time);
    text_layer_set_text(city_layer, out_text);
    if (units_changed & HOUR_UNIT)
    {
        req_status = REQ_NOT_SENT;
    }
    if (req_status == REQ_NOT_SENT)
    {
        send_city_request(hour);
    }
}

static void send_city_request(int hour)
{
    Tuplet hour_tuple = TupletInteger(CITY_KEY_HOUR, hour);

    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (iter == NULL)
    {
        return;
    }

    dict_write_tuplet(iter, &hour_tuple);
    dict_write_end(iter);

    app_message_outbox_send();
    req_status = REQ_SENT;
    app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "App Message Request Sent [hour: %d]!", hour);
}

static void handle_in_received(DictionaryIterator *iter, void *context)
{
    app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "App Message Received!");
    Tuple *hour_tuple = dict_find(iter, CITY_KEY_HOUR);
    Tuple *name_tuple = dict_find(iter, CITY_KEY_NAME);
    
    int hour = hour_tuple->value->int32;
    char *name = name_tuple->value->cstring;
    
    save_city(hour, name, sizeof(name));
    if (city_hour == hour)
    {
        strncpy(city_name, name, sizeof(name));
    }
    req_status = RESP_RECEIVED;
}

static void handle_in_dropped(AppMessageResult reason, void *context)
{
    req_status = REQ_NOT_SENT;
    app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "App Message Dropped!");
}

static void get_reason_str(char * reason_str, AppMessageResult reason, int reason_str_size)
{
    if (reason == APP_MSG_OK)
    {
        strncpy(reason_str, "APP_MSG_OK", reason_str_size);
    }
    else if (reason == APP_MSG_SEND_TIMEOUT)
    {
        strncpy(reason_str, "APP_MSG_SEND_TIMEOUT", reason_str_size);
    }
    else if (reason == APP_MSG_SEND_REJECTED)
    {
        strncpy(reason_str, "APP_MSG_SEND_REJECTED", reason_str_size);
    }
    else if (reason == APP_MSG_NOT_CONNECTED)
    {
        strncpy(reason_str, "APP_MSG_NOT_CONNECTED", reason_str_size);
    }
    else if (reason == APP_MSG_APP_NOT_RUNNING)
    {
        strncpy(reason_str, "APP_MSG_APP_NOT_RUNNING", reason_str_size);
    }
    else if (reason == APP_MSG_INVALID_ARGS)
    {
        strncpy(reason_str, "APP_MSG_INVALID_ARGS", reason_str_size);
    }
    else if (reason == APP_MSG_BUSY)
    {
        strncpy(reason_str, "APP_MSG_BUSY", reason_str_size);
    }
    else if (reason == APP_MSG_BUFFER_OVERFLOW)
    {
        strncpy(reason_str, "APP_MSG_BUFFER_OVERFLOW", reason_str_size);
    }
    else if (reason == APP_MSG_ALREADY_RELEASED)
    {
        strncpy(reason_str, "APP_MSG_ALREADY_RELEASED", reason_str_size);
    }
    else if (reason == APP_MSG_CALLBACK_ALREADY_REGISTERED)
    {
        strncpy(reason_str, "APP_MSG_CALLBACK_ALREADY_REGISTERED", reason_str_size);
    }
    else if (reason == APP_MSG_CALLBACK_NOT_REGISTERED)
    {
        strncpy(reason_str, "APP_MSG_CALLBACK_NOT_REGISTERED", reason_str_size);
    }
    else if (reason == APP_MSG_OUT_OF_MEMORY)
    {
        strncpy(reason_str, "APP_MSG_OUT_OF_MEMORY", reason_str_size);
    }
    else if (reason == APP_MSG_CLOSED)
    {
        strncpy(reason_str, "APP_MSG_CLOSED", reason_str_size);
    }
    else if (reason == APP_MSG_INTERNAL_ERROR)
    {
        strncpy(reason_str, "APP_MSG_INTERNAL_ERROR", reason_str_size);
    }
    else
    {
        strncpy(reason_str, "", reason_str_size);
    }
}

static void handle_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
    char reason_str[40] = "";
    get_reason_str(reason_str, reason, sizeof(reason_str));
    req_status = REQ_NOT_SENT;
    app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "App Message Failed to Send! %s", reason_str);
}

static void app_message_init(void)
{
    // Register message handlers
    app_message_register_inbox_received(handle_in_received);
    app_message_register_inbox_dropped(handle_in_dropped);
    app_message_register_outbox_failed(handle_out_failed);
    // Init buffers
    app_message_open(64, 64);
}

// Handle the start-up of the app
static void do_init(void)
{
    // Prepare to send and receive app messages
    app_message_init();
    
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
