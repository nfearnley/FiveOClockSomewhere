static void load_city(int hour);
static void save_city();
static void download_city(int hour);
static void get_text(char *out_text, struct tm* tick_time);
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed);
static void do_init(void);
static void do_deinit(void);
int main(void);
