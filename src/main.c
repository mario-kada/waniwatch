#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_layer;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;



static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  static char date_buffer[16]; 
strftime(date_buffer, sizeof(date_buffer), "%m/%d", tick_time);
  
    static char ss_buffer[16]; 
strftime(ss_buffer, sizeof(ss_buffer), " %a", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
  text_layer_set_text(s_layer, ss_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(60, 60), bounds.size.w, 50));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  
  // Create date TextLayer
  s_date_layer = text_layer_create(
      GRect(PBL_IF_ROUND_ELSE(46, 28), PBL_IF_ROUND_ELSE(45, 45), bounds.size.w, 50));
  
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  
    // Create s_layer TextLayer
  s_layer = text_layer_create(
      GRect(PBL_IF_ROUND_ELSE(30,28),  PBL_IF_ROUND_ELSE(45, 45), bounds.size.w, 50));
  
  text_layer_set_text_color(s_layer, GColorWhite);
  text_layer_set_background_color(s_layer, GColorClear);
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);
  text_layer_set_font(s_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
   
  
  // Add to Window
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WANI);
  s_bitmap_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_background_color(s_bitmap_layer, GColorVividCerulean);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_layer));

}



static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_layer);
  bitmap_layer_destroy(s_bitmap_layer);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}