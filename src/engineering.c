#include <pebble.h>
#include <inttypes.h>
#include "engineering.h"
#include "libshadow.h"

static Window *window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_day_label, *s_num_label, *s_steps_label;

static GPath *s_minute_arrow, *s_hour_arrow;
static char s_date_buffer[7], s_temp_buffer[5], s_steps_buffer[8];


static AppSync s_sync;
static uint8_t s_sync_buffer[64];

static GColor gcolor_background, gcolor_hour_marks, gcolor_minute_marks, gcolor_numbers, gcolor_hour_hand, gcolor_minute_hand, gcolor_second_hand;
static bool b_show_numbers, b_show_temperature, b_show_date, b_show_second_hand, b_show_steps;

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(window_get_root_layer(window));
}

static GShadow shadow_bg, shadow_hour_marks, shadow_minute_marks, shadow_numbers, shadow_hour_hand, shadow_minute_hand, shadow_second_hand, shadow_dot, shadow_date_box, shadow_hole;

static void load_persisted_values() {
	if (persist_exists(KEY_SHOW_NUMBERS)) {
	  b_show_numbers = persist_read_int(KEY_SHOW_NUMBERS);
	}

	// SHOW_SECOND_HAND
	if (persist_exists(KEY_SHOW_SECOND_HAND)) {
	  b_show_second_hand = persist_read_int(KEY_SHOW_SECOND_HAND);
	}

	// SHOW_TEMPERATURE
	if (persist_exists(KEY_SHOW_TEMPERATURE)) {
	  b_show_temperature = persist_read_int(KEY_SHOW_TEMPERATURE);
	}

	// SHOW_TEMPERATURE
	if (persist_exists(KEY_SHOW_TEMPERATURE)) {
	  b_show_temperature = persist_read_int(KEY_SHOW_TEMPERATURE);
	}

	// SHOW_STEPS
	if (persist_exists(KEY_SHOW_STEPS)) {
	  b_show_steps = persist_read_int(KEY_SHOW_STEPS);
	}

	// SHOW_DATE
	if (persist_exists(KEY_SHOW_DATE)) {
	  b_show_date = persist_read_int(KEY_SHOW_DATE);
	}

	// COLOR_BACKGROUND
	if (persist_exists(KEY_COLOR_BACKGROUND)) {
		int color_hex = persist_read_int(KEY_COLOR_BACKGROUND);
		gcolor_background = GColorFromHEX(color_hex);
		window_set_background_color(window, gcolor_background);
	}

	// COLOR_HOUR_MARKS
	if (persist_exists(KEY_COLOR_HOUR_MARKS)) {
		int color_hex = persist_read_int(KEY_COLOR_HOUR_MARKS);
		gcolor_hour_marks = GColorFromHEX(color_hex);
	}

	// COLOR_MINUTE_MARKS
	if (persist_exists(KEY_COLOR_MINUTE_MARKS)) {
		int color_hex = persist_read_int(KEY_COLOR_MINUTE_MARKS);
		gcolor_minute_marks = GColorFromHEX(color_hex);
	}

	// COLOR_LABEL
	if (persist_exists(KEY_COLOR_LABEL)) {
		int color_hex = persist_read_int(KEY_COLOR_LABEL);
		gcolor_numbers = GColorFromHEX(color_hex);
	}

	// COLOR_HOUR_HAND
	if (persist_exists(KEY_COLOR_HOUR_HAND)) {
		int color_hex = persist_read_int(KEY_COLOR_HOUR_HAND);
		gcolor_hour_hand = GColorFromHEX(color_hex);
	}

	// COLOR_MINUTE_HAND
	if (persist_exists(KEY_COLOR_MINUTE_HAND)) {
		int color_hex = persist_read_int(KEY_COLOR_MINUTE_HAND);
		gcolor_minute_hand = GColorFromHEX(color_hex);
	}

	// COLOR_SECOND_HAND
	if (persist_exists(KEY_COLOR_SECOND_HAND)) {
		int color_hex = persist_read_int(KEY_COLOR_SECOND_HAND);
		gcolor_second_hand = GColorFromHEX(color_hex);
	}
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *temperature_t = dict_find(iter, KEY_TEMPERATURE);
  if(temperature_t) {
    snprintf(s_temp_buffer, 5, "%d°", temperature_t->value->int16);
    APP_LOG(APP_LOG_LEVEL_INFO, s_temp_buffer);
  }

  Tuple *show_numbers_t = dict_find(iter, KEY_SHOW_NUMBERS);
  if(show_numbers_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Show numbers %d", show_numbers_t->value->uint8);
    b_show_numbers = show_numbers_t->value->uint8;
    persist_write_int(KEY_SHOW_NUMBERS, b_show_numbers);
  }

  Tuple *show_date_t = dict_find(iter, KEY_SHOW_DATE);
  if(show_date_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Show date %d", show_date_t->value->uint8);
    b_show_date = show_date_t->value->uint8;
    persist_write_int(KEY_SHOW_DATE, show_date_t->value->uint8);
  }

  Tuple *show_temperature_t = dict_find(iter, KEY_SHOW_TEMPERATURE);
  if(show_temperature_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Show temperature %d", show_temperature_t->value->uint8);
    b_show_temperature = show_temperature_t->value->uint8;
    persist_write_int(KEY_SHOW_TEMPERATURE, b_show_temperature);
  }

  Tuple *show_steps_t = dict_find(iter, KEY_SHOW_STEPS);
	if(show_steps_t) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Show steps %d", show_steps_t->value->uint8);
 		b_show_steps = show_steps_t->value->uint8;
		persist_write_int(KEY_SHOW_STEPS, show_steps_t->value->uint8);
 	}

  Tuple *show_second_hand_t = dict_find(iter, KEY_SHOW_SECOND_HAND);
  if(show_second_hand_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Show second hand %d", show_second_hand_t->value->uint8);
    b_show_second_hand = show_second_hand_t->value->uint8;
    persist_write_int(KEY_SHOW_SECOND_HAND, show_second_hand_t->value->uint8);
  }

  Tuple *color_background_t = dict_find(iter, KEY_COLOR_BACKGROUND);
  if(color_background_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Background color %lu", color_background_t->value->int32);
    gcolor_background = GColorFromHEX(color_background_t->value->int32);
    window_set_background_color(window, gcolor_background);
    persist_write_int(KEY_COLOR_BACKGROUND, color_background_t->value->int32);
  }

  Tuple *color_label_t = dict_find(iter, KEY_COLOR_LABEL);
  if(color_label_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Label color %lu", color_label_t->value->int32);
    gcolor_numbers = GColorFromHEX(color_label_t->value->int32);
    persist_write_int(KEY_COLOR_LABEL, color_label_t->value->int32);
  }

  Tuple *color_hour_marks_t = dict_find(iter, KEY_COLOR_HOUR_MARKS);
  if(color_hour_marks_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Hour mark color %lu", color_hour_marks_t->value->int32);
    gcolor_hour_marks = GColorFromHEX(color_hour_marks_t->value->int32);
    persist_write_int(KEY_COLOR_HOUR_MARKS, color_hour_marks_t->value->int32);
  }

  Tuple *color_minute_marks_t = dict_find(iter, KEY_COLOR_MINUTE_MARKS);
  if(color_minute_marks_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Minute mark color %lu", color_minute_marks_t->value->int32);
    gcolor_minute_marks = GColorFromHEX(color_minute_marks_t->value->int32);
    persist_write_int(KEY_COLOR_MINUTE_MARKS, color_minute_marks_t->value->int32);
  }

  Tuple *color_hour_hand_t = dict_find(iter, KEY_COLOR_HOUR_HAND);
  if(color_hour_hand_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Hour hand color %lu", color_hour_hand_t->value->int32);
    gcolor_hour_hand = GColorFromHEX(color_hour_hand_t->value->int32);
    persist_write_int(KEY_COLOR_HOUR_HAND, color_hour_hand_t->value->int32);
  }

  Tuple *color_minute_hand_t = dict_find(iter, KEY_COLOR_MINUTE_HAND);
  if(color_minute_hand_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Minute hand color %lu", color_minute_hand_t->value->int32);
    gcolor_minute_hand = GColorFromHEX(color_minute_hand_t->value->int32);
    persist_write_int(KEY_COLOR_MINUTE_HAND, color_minute_hand_t->value->int32);
  }

  Tuple *color_second_hand_t = dict_find(iter, KEY_COLOR_SECOND_HAND);
  if(color_second_hand_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Second hand color %lu", color_second_hand_t->value->int32);
    gcolor_second_hand = GColorFromHEX(color_second_hand_t->value->int32);
    persist_write_int(KEY_COLOR_SECOND_HAND, color_second_hand_t->value->int32);
  }
}

static bool color_hour_marks(GDrawCommand *command, uint32_t index, void *context) {
  gdraw_command_set_stroke_color(command, gcolor_hour_marks);
  return true;
}

static bool color_minute_marks(GDrawCommand *command, uint32_t index, void *context) {
  gdraw_command_set_stroke_color(command, gcolor_minute_marks);
  return true;
}


static int32_t get_angle_for_hour(int hour) {
  // Progress through 12 hours, out of 360 degrees
  return (hour * 360) / 12;
}

static int32_t get_angle_for_minute(int hour) {
  // Progress through 60 miunutes, out of 360 degrees
  return (hour * 360) / 60;
}


static void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GRect frame = bounds; /////// grect_inset(bounds, GEdgeInsets(4 * INSET));
  GRect inner_hour_frame = grect_inset(bounds, GEdgeInsets((4 * INSET) + 8));
  GRect inner_minute_frame = grect_inset(bounds, GEdgeInsets((4 * INSET) + 6));

  // background
  switch_to_shadow_ctx (ctx);{
    graphics_context_set_fill_color(ctx, gcolor (shadow_bg));
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  }revert_to_fb_ctx (ctx);

  // Center hole
  switch_to_shadow_ctx (ctx);{
    graphics_context_set_fill_color(ctx, gcolor (shadow_hole));
    graphics_fill_circle(ctx, (GPoint){.x = bounds.size.w / 2, .y = bounds.size.h / 2}, bounds.size.w / 2 - (4 * INSET + 13));
  }revert_to_fb_ctx (ctx);

  // numbers
  if (b_show_numbers) {
    switch_to_shadow_ctx (ctx);{
      graphics_context_set_text_color(ctx, gcolor (shadow_numbers));
#ifdef PBL_RECT
      graphics_draw_text(ctx, "12", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(63, 18, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_text(ctx, "1", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(85, 23, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "2", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(104, 43, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "3", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(112, 68, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "4", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(104, 93, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "5", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(85, 110, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "6", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(62, 118, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_text(ctx, "7", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(39, 110, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "8", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(20, 93, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "9", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(14, 68, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "10", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(20, 43, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "11", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(39, 23, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
#else
      graphics_draw_text(ctx, "12", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(80, 10, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_text(ctx, "1", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(107, 20, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "2", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(130, 43, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "3", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(140, 74, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "4", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(130, 106, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "5", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(107, 126, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, "6", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(81, 136, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_text(ctx, "7", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(53, 124, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "8", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(29, 106, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "9", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(20, 74, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "10", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(28, 42, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, "11", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(50, 22, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
#endif
    }revert_to_fb_ctx (ctx);
    graphics_context_set_text_color(ctx, gcolor_numbers);
#ifdef PBL_RECT
    graphics_draw_text(ctx, "12", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(63, 18, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, "1", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(85, 23, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "2", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(104, 43, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "3", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(112, 68, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "4", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(104, 93, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "5", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(85, 110, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "6", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(62, 118, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, "7", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(39, 110, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "8", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(20, 93, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "9", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(14, 68, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "10", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(20, 43, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "11", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(39, 23, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
#else
    graphics_draw_text(ctx, "12", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(80, 10, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, "1", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(107, 20, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "2", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(130, 43, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "3", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(140, 74, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "4", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(130, 106, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "5", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(107, 126, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    graphics_draw_text(ctx, "6", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(81, 136, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, "7", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(53, 124, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "8", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(29, 106, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "9", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(20, 74, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "10", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(28, 42, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, "11", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(50, 22, 20, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
#endif
  }


  // Hours marks
  graphics_context_set_stroke_width(ctx, 3);
  switch_to_shadow_ctx (ctx);{
    graphics_context_set_stroke_color(ctx, gcolor (shadow_hour_marks));
    for(int i = 0; i < 12; i++) {
      int hour_angle = get_angle_for_hour(i);
      GPoint p0 = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));
      GPoint p1 = gpoint_from_polar(inner_hour_frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));
      graphics_draw_line(ctx, p0, p1);
    }
  }revert_to_fb_ctx (ctx);
  graphics_context_set_stroke_color(ctx, gcolor_hour_marks);
  for(int i = 0; i < 12; i++) {
    int hour_angle = get_angle_for_hour(i);
    GPoint p0 = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));
    GPoint p1 = gpoint_from_polar(inner_hour_frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));
    graphics_draw_line(ctx, p0, p1);
  }

  // Minute Marks
  graphics_context_set_stroke_width(ctx, 1);
  switch_to_shadow_ctx (ctx);{
    graphics_context_set_stroke_color(ctx, gcolor (shadow_minute_marks));
    for(int i = 0; i < 60; i++) {
      if (i % 5) {
        int minute_angle = get_angle_for_minute(i);
        GPoint p0 = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
        GPoint p1 = gpoint_from_polar(inner_minute_frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
        graphics_draw_line(ctx, p0, p1);
      }
    }
  }revert_to_fb_ctx (ctx);
  graphics_context_set_stroke_color(ctx, gcolor_minute_marks);
  for(int i = 0; i < 60; i++) {
    if (i % 5) {
      int minute_angle = get_angle_for_minute(i);
      GPoint p0 = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
      GPoint p1 = gpoint_from_polar(inner_minute_frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
      graphics_draw_line(ctx, p0, p1);
    }
  }

}

static void health_handler(HealthEventType event, void *context) {

	if (b_show_steps) {
		if (event == HealthEventMovementUpdate) {
			// display the step count

			//Lifted from https://github.com/freakified/TimeStylePebble by freakified
			// format step string
      int steps = (int)health_service_sum_today(HealthMetricStepCount);
      if(steps < 1000) {
        snprintf(s_steps_buffer, sizeof(s_steps_buffer), "%i", steps);
      } else {
        int steps_thousands = steps / 1000;
        int steps_hundreds  = steps / 100 % 10;

        char decimalSeparator[1] = ".";

        if (steps < 10000) {
          snprintf(s_steps_buffer, sizeof(s_steps_buffer), "%i%s%iK", steps_thousands, decimalSeparator, steps_hundreds);
        } else {
          snprintf(s_steps_buffer, sizeof(s_steps_buffer), "%iK", steps_thousands);
        }
      }
		
			text_layer_set_text(s_steps_label, s_steps_buffer);

		}
	}

  
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int16_t second_hand_length = bounds.size.w / 2 - 5;
  int16_t second_hand_tail_length = 15;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  int32_t second_angle_tail = TRIG_MAX_ANGLE * (t->tm_sec + 30) / 60;

  GPoint second_hand = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  GPoint second_hand_tail = {
    .x = (int16_t)(sin_lookup(second_angle_tail) * (int32_t)second_hand_tail_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle_tail) * (int32_t)second_hand_tail_length / TRIG_MAX_RATIO) + center.y,
  };

  // date
  if (b_show_date) {
    graphics_context_set_text_color(ctx, gcolor_numbers);
    int offset = !b_show_numbers * 10;
    const GRect date_rect = (GRect){.origin = (GPoint){.x = bounds.size.w / 2 + 10, .y = bounds.size.h / 2 - PBL_IF_RECT_ELSE(14, 18) / 2},
                                    .size = (GSize){.w = PBL_IF_RECT_ELSE(40, 45) + offset, .h = PBL_IF_RECT_ELSE(14, 18)}};
    graphics_draw_text(ctx, s_date_buffer, fonts_get_system_font(PBL_IF_RECT_ELSE(FONT_KEY_GOTHIC_14, FONT_KEY_GOTHIC_18)), date_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    switch_to_shadow_ctx (ctx);{
      graphics_context_set_fill_color(ctx, gcolor (shadow_date_box));
      graphics_fill_rect(ctx, grect_inset (date_rect, GEdgeInsets(0,-1,PBL_IF_RECT_ELSE(-3, -4))), 0, GCornerNone);
    }revert_to_fb_ctx (ctx);
  }

  // temperature
  if (b_show_temperature) {
    graphics_context_set_text_color(ctx, gcolor_numbers);
    int offset = !b_show_numbers * 10;
    // steps
    if (!b_show_steps) {

      health_service_events_unsubscribe();
      text_layer_set_text(s_steps_label, "");
    } else {
      text_layer_set_text_color(s_steps_label, gcolor_numbers);

      // subscribe to health events
	  	if(health_service_events_subscribe(health_handler, NULL)) {
	    	health_handler(HealthEventMovementUpdate, NULL);
	  	} else {
	    	APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
	  	}
    }

#ifdef PBL_RECT
    graphics_draw_text(ctx, s_temp_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(27 - offset, 75, 40 + offset, 14), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
#else
    graphics_draw_text(ctx, s_temp_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(40 - offset, 78, 40 + offset, 14), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
#endif
  }

  // minute hand
  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  switch_to_shadow_ctx (ctx);{
    graphics_context_set_fill_color(ctx, gcolor (shadow_minute_hand));
    gpath_draw_filled(ctx, s_minute_arrow);
  }revert_to_fb_ctx (ctx);
  graphics_context_set_fill_color(ctx, gcolor_minute_hand);
  gpath_draw_filled(ctx, s_minute_arrow);

  // hour hand
  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  switch_to_shadow_ctx (ctx);{
    graphics_context_set_fill_color(ctx, gcolor (shadow_hour_hand));
    gpath_draw_filled(ctx, s_hour_arrow);
  }revert_to_fb_ctx (ctx);
  graphics_context_set_fill_color(ctx, gcolor_hour_hand);
  gpath_draw_filled(ctx, s_hour_arrow);

  if (b_show_second_hand) {
    switch_to_shadow_ctx (ctx);{
      graphics_context_set_fill_color(ctx, gcolor (shadow_second_hand));
      graphics_draw_line(ctx, second_hand, center);
      graphics_draw_line(ctx, second_hand_tail, center);
    }revert_to_fb_ctx (ctx);
    graphics_context_set_stroke_color(ctx, gcolor_second_hand);
    graphics_draw_line(ctx, second_hand, center);
    graphics_draw_line(ctx, second_hand_tail, center);
  }

  switch_to_shadow_ctx (ctx);{
    graphics_context_set_fill_color(ctx, gcolor (shadow_dot));
    graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), 4);
  }revert_to_fb_ctx (ctx);
  graphics_context_set_fill_color(ctx, gcolor_second_hand);
  graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), 4);

  create_shadow (ctx, NW);
  reset_shadow ();
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_date_buffer, sizeof(s_date_buffer), "%a %d", t);
  uppercase(s_date_buffer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  shadow_bg = new_shadowing_object (0, 3, 0);
  shadow_hour_marks = new_shadowing_object (0, 1,0);
  shadow_minute_marks = new_shadowing_object (0, 1, 0);
  shadow_numbers = new_shadowing_object (-4, 1, 0);

  shadow_minute_hand = new_shadowing_object (-1, 2, 2);
  shadow_hour_hand = new_shadowing_object (0, 2, 4);
  shadow_second_hand = new_shadowing_object (0, 1, 4);
  shadow_dot = new_shadowing_object (-4, -1, 4);
  shadow_date_box = new_shadowing_object (-4, -2, 0);
  shadow_hole = new_shadowing_object (-5, 0, 0);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  window_set_background_color(window, gcolor_background);

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);

  load_persisted_values();

	load_persisted_values();

	if (b_show_second_hand) {

		tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	} else {

		tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	}

	// subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display

		int offset = !b_show_numbers * 10;

    s_steps_label = text_layer_create(PBL_IF_ROUND_ELSE(
                                                        GRect(0, 120 + offset, 180, 18),
                                                        GRect(0, 100 + offset, 144, 14)
                                                        ));

    text_layer_set_background_color(s_steps_label, GColorClear);
    text_layer_set_text_color(s_steps_label, gcolor_numbers);
    text_layer_set_font(s_steps_label, fonts_get_system_font(PBL_IF_ROUND_ELSE(FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_14)));
    text_layer_set_text_alignment(s_steps_label, GTextAlignmentCenter);
    layer_add_child(s_date_layer, text_layer_get_layer(s_steps_label));

    if (b_show_steps) {
      health_handler(HealthEventMovementUpdate, NULL);
    }

  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_date_layer);

  text_layer_destroy(s_day_label);
  text_layer_destroy(s_num_label);

	text_layer_destroy(s_steps_label);

  destroy_shadow_ctx ();
}

static void init() {

  // Default colors
  gcolor_background = GColorOxfordBlue;
  gcolor_minute_hand = GColorChromeYellow;

#ifdef PBL_COLOR
  gcolor_hour_marks = GColorWhite;
  gcolor_minute_marks = GColorWhite;
  gcolor_numbers = GColorChromeYellow;
  gcolor_hour_hand = GColorWhite;
  gcolor_second_hand = GColorChromeYellow;
#else
  gcolor_hour_marks = GColorWhite;
  gcolor_minute_marks = GColorWhite;
  gcolor_numbers = GColorWhite;
  gcolor_hour_hand = GColorWhite;
  gcolor_second_hand = GColorWhite;
#endif

  b_show_numbers = true;
  b_show_second_hand = true;
  b_show_date = true;
  b_show_temperature = true;
  b_show_steps = true;

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
        .unload = window_unload,
        });
  window_stack_push(window, true);

  s_temp_buffer[0] = '\0';
  s_date_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(64, 64);
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

	tick_timer_service_unsubscribe();
	health_service_events_unsubscribe();

	window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}

char *uppercase(char *str) {
  for (int i = 0; str[i] != 0; i++) {
    if (str[i] >= 'a' && str[i] <= 'z') {
      str[i] -= 0x20;
    }
  }

  return str;
}
