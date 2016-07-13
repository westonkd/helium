#include <pebble.h>

#define HOUR_RAD 60
#define MINUTE_RAD 35
#define DATE_RAD 15
#define MARGIN 8

Window *s_main_window;
static Layer *s_canvas_layer; //Drawing Canvas
static GFont s_font;
static int s_battery_level;
static bool is_connected;
static GBitmap *bitmap_blue;

/***********************************************
* Bluetooth Hanlder
***********************************************/
static void bluetooth_callback(bool connected) {
  is_connected = connected;
  
  if (!connected) {
    vibes_double_pulse();
  }
  
  layer_mark_dirty(s_canvas_layer);
}

static void draw_bluetooth(GContext *ctx, GPoint center) {
  if (!is_connected) {
    // Draw the circle
    graphics_draw_circle(ctx, GPoint(DATE_RAD + MARGIN, DATE_RAD + MARGIN), DATE_RAD);
    
    // Get the bounds of the image
    GRect bitmap_bounds = gbitmap_get_bounds(bitmap_blue);
    
    bitmap_bounds.origin = GPoint(DATE_RAD, DATE_RAD);
    
    // Set the compositing mode (GCompOpSet is required for transparency)
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    
    // Draw the image
    graphics_draw_bitmap_in_rect(ctx, bitmap_blue, bitmap_bounds);
    
  }
}

/***********************************************
* Draws a circle at the specified coordinates
* using the specified radius and context
***********************************************/
static void draw_indicator(int x, int y, uint16_t radius, GContext *ctx) {
  // Get the center and rad
  GPoint center = GPoint(x, y);
  graphics_draw_circle(ctx, center, radius);

  // Fill a circle
  graphics_fill_circle(ctx, center, radius);
}

/***********************************************
* Helpers to get coordinates
***********************************************/
static float get_hour_x(int hour, GPoint center) {
  if (hour < 0 || hour > 12) {
    return -1.0;
  }
  
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  int32_t second_angle = TRIG_MAX_ANGLE * hour / 12;
  return (sin_lookup(second_angle) * HOUR_RAD / TRIG_MAX_RATIO) + center.x;
}

static float get_hour_y(int hour, GPoint center) {
  if (hour < 0 || hour > 12) {
    return -1.0;
  }
  
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  int32_t second_angle = TRIG_MAX_ANGLE * hour / 12;
  return (-cos_lookup(second_angle) * HOUR_RAD / TRIG_MAX_RATIO) + center.y;
}

static float get_minute_x(int minute, GPoint center) {
  if (minute < 0 || minute > 60) {
    return -1.0;
  }
  
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  int32_t second_angle = TRIG_MAX_ANGLE * minute / 60;
  return (sin_lookup(second_angle) * MINUTE_RAD / TRIG_MAX_RATIO) + center.x;  
}

static float get_minute_y(int minute, GPoint center) {
  if (minute < 0 || minute > 60) {
    return -1.0;
  }
  
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  int32_t second_angle = TRIG_MAX_ANGLE * minute / 60;
  return (-cos_lookup(second_angle) * MINUTE_RAD / TRIG_MAX_RATIO) + center.y;
}

/***********************************************
* Draw the Date circle and text
***********************************************/
static void draw_date(GContext *ctx, GPoint center) {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  GRect bounds = layer_get_bounds(s_canvas_layer);
  
  // Draw a whole circle
  graphics_draw_circle(ctx, center, DATE_RAD);
  graphics_fill_circle(ctx, center, DATE_RAD);
  
  // Set the font color
  graphics_context_set_text_color(ctx, GColorBlack);
  
  // Get the day
  static char date_buffer[5];
  strftime(date_buffer, sizeof(date_buffer), "%d", tick_time);
  
  // Draw the text
  graphics_draw_text(ctx, date_buffer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS), GRect((bounds.size.w / 2) - DATE_RAD, (bounds.size.h / 2) - DATE_RAD + 2, DATE_RAD * 2, DATE_RAD * 2), GTextOverflowModeWordWrap, GTextAlignmentCenter , NULL);
}

/***********************************************
* Battery draw and callback
***********************************************/
static void draw_battery(GContext *ctx, GPoint center) {
  graphics_draw_circle(ctx, GPoint(center.x * 2 - DATE_RAD - MARGIN, center.y * 2 - DATE_RAD - MARGIN), DATE_RAD);
  
  graphics_context_set_text_color(ctx, GColorWhite);
  
  static char buf[5];
  snprintf(buf, sizeof(buf), "%d", s_battery_level);
  graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(center.x * 2 - DATE_RAD * 2 - MARGIN, center.y * 2 - DATE_RAD * 2 - MARGIN + 2, DATE_RAD * 2, DATE_RAD * 2), GTextOverflowModeWordWrap, GTextAlignmentCenter , NULL);
}

static void battery_callback(BatteryChargeState state) {
  //Record the new battery level
  s_battery_level = state.charge_percent;
  
  // Redraw
  layer_mark_dirty(s_canvas_layer);
}

/***********************************************
* Update the canvas layer
***********************************************/
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int hour = tick_time->tm_hour % 12;
  int minute = tick_time->tm_min;
  
  // If the hour is 1
  if (hour == 0) {
    hour = 12;
  }
  
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
  draw_indicator((int) get_hour_x(hour,center),(int) get_hour_y(hour,center),5,ctx);
  
  // Draw the minute circle
  draw_indicator((int) get_minute_x(minute, center),(int) get_minute_y(minute, center),3,ctx);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Hour %d", hour);
  
  // Draw the date
  draw_date(ctx, center);
  
  // Draw bettery
  draw_battery(ctx, center);
  
  // Draw bluetooth indicator
  draw_bluetooth(ctx, center);
}

static void update_time() {
  // Mark Dirty
  layer_mark_dirty(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

/***********************************************
* Load the main window
***********************************************/
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
  
  // Initialize battery indicator
  battery_callback(battery_state_service_peek());
  
  // Initialize Bluetooth moniter
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  bitmap_blue = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUE);

  
  // Redraw this as soon as possible
  layer_mark_dirty(s_canvas_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

/***********************************************
* Initialize
***********************************************/
void handle_init(void) {
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Register the time change handler
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Load font
  s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SOURCE_CODE_PRO_16));
  
  // Register Battery handler and initialize 
  battery_state_service_subscribe(battery_callback);
  
  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
  
  window_stack_push(s_main_window, true);
}

void handle_deinit(void) {
  window_destroy(s_main_window);
  
  gbitmap_destroy(bitmap_blue);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
