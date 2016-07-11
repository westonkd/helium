#include <pebble.h>

#define HOUR_RAD 60
#define MINUTE_RAD 35

Window *s_main_window;
static Layer *s_canvas_layer; //Drawing Canvas

static void draw_indicator(int x, int y, uint16_t radius, GContext *ctx) {
  // Get the center and rad
  GPoint center = GPoint(x, y);
  graphics_draw_circle(ctx, center, radius);

  // Fill a circle
  graphics_fill_circle(ctx, center, radius);
  
  // Mark Dirty
  layer_mark_dirty(s_canvas_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int hour = tick_time->tm_hour % 12;
  
  // If the hour is 1
  if (hour == 0) {
    hour = 1;
  }
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "hour: %d", hour);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  // Set the line color
  graphics_context_set_stroke_color(ctx, GColorWhite);
  // Set the fill color
  graphics_context_set_fill_color(ctx, GColorWhite);
  // Set the stroke width (must be an odd integer value)
  graphics_context_set_stroke_width(ctx, 5);
  
  // Draw the hour orbital
  GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
  uint16_t hour_radius = HOUR_RAD;
  graphics_draw_circle(ctx, center, hour_radius);  
  
  // Draw the minute orbital
  uint16_t minute_radius = MINUTE_RAD;
  graphics_draw_circle(ctx, center, minute_radius);
  
  // Draw the hour circle
  draw_indicator(25,25,5,ctx);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  
  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  // Change background to black
  window_set_background_color(s_main_window, GColorBlack);
  
  // Add to Window
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  
  // Redraw this as soon as possible
  layer_mark_dirty(s_canvas_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

void handle_init(void) {
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Register the time change handler
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  window_stack_push(s_main_window, true);
}

void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
