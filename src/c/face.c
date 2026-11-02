#include "face.h"
#include "bonsai.h"
#include "settings.h"

static Window *s_window;
static Layer *s_layer;

static GBitmap *s_stages[STAGE_COUNT];
static bool s_bitmaps_loaded;

static int s_hour;
static int s_minute;
static int s_wday;
static int s_mday;
static int s_mon;
static int s_pulse;

static void unload_stages(void) {
  for (int i = 0; i < STAGE_COUNT; i++) {
    if (s_stages[i]) {
      gbitmap_destroy(s_stages[i]);
      s_stages[i] = NULL;
    }
  }
  s_bitmaps_loaded = false;
}

static void load_stages(void) {
  if (s_bitmaps_loaded) {
    return;
  }
  for (int i = 0; i < STAGE_COUNT; i++) {
    s_stages[i] = gbitmap_create_with_resource(bonsai_stage_resource((GrowthStage)i));
  }
  s_bitmaps_loaded = true;
}

static bool use_24h(void) {
  const FaceSettings *s = settings_get();
  if (s->time_format == TIME_FMT_24H) return true;
  if (s->time_format == TIME_FMT_12H) return false;
  return clock_is_24h_style();
}

static void format_time(char *buf, size_t n) {
  int hour = s_hour;
  const bool is24 = use_24h();
  if (!is24) {
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }
  if (settings_get()->leading_zero) {
    snprintf(buf, n, "%02d:%02d", hour, s_minute);
  } else {
    snprintf(buf, n, "%d:%02d", hour, s_minute);
  }
}

static void format_date(char *buf, size_t n) {
  static const char *const DOW[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char *const MON[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  snprintf(buf, n, "%s %d %s", DOW[s_wday % 7], s_mday, MON[s_mon % 12]);
}

static void update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const BonsaiState *tree = bonsai_get();
  const FaceSettings *cfg = settings_get();
  GColor bg = bonsai_bg_color();
  GColor fg = bonsai_fg_color();

  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  const int caption_h = 42;
  const int time_bottom = cfg->show_date ? 64 : 48;

  GBitmap *sprite = s_stages[tree->stage];
  if (sprite) {
    GSize sz = gbitmap_get_bounds(sprite).size;
    // Gentle sway when the tree has any foliage.
    int sway = 0;
    if (tree->stage > STAGE_BARE) {
      sway = ((s_pulse % 8) < 4) ? 1 : 0;
    }

    // Draw at native bitmap size only — dest larger than the bitmap tiles
    // on Pebble (no upscaling). Center in the band between time and captions.
    const int band_top = time_bottom;
    const int band_bot = bounds.size.h - caption_h;
    const int band_h = band_bot - band_top;
    int tree_y = band_top + (band_h - sz.h) / 2;
    if (tree_y < band_top) {
      tree_y = band_top;
    }
    GRect dest = GRect((bounds.size.w - sz.w) / 2 + sway, tree_y, sz.w, sz.h);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, sprite, dest);
  }

  char time_buf[16];
  format_time(time_buf, sizeof(time_buf));
  graphics_context_set_text_color(ctx, fg);
  graphics_draw_text(ctx, time_buf,
                     fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
                     GRect(0, 2, bounds.size.w, 44),
                     GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);

  if (cfg->show_date) {
    char date_buf[24];
    format_date(date_buf, sizeof(date_buf));
    graphics_draw_text(ctx, date_buf,
                       fonts_get_system_font(FONT_KEY_GOTHIC_14),
                       GRect(0, 44, bounds.size.w, 18),
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
  }

  if (cfg->show_stage_label) {
    graphics_context_set_text_color(ctx, fg);
    graphics_draw_text(ctx, bonsai_stage_label(tree->stage),
                       fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                       GRect(0, bounds.size.h - 40, bounds.size.w, 22),
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
  }

  if (cfg->show_steps) {
    char step_buf[32];
    if (tree->has_steps) {
      snprintf(step_buf, sizeof(step_buf), "%d / %d", tree->steps, tree->goal);
    } else if (cfg->demo_mode) {
      snprintf(step_buf, sizeof(step_buf), "%d · demo", tree->steps);
    } else {
      snprintf(step_buf, sizeof(step_buf), "No steps");
    }
    graphics_context_set_text_color(ctx, fg);
    graphics_draw_text(ctx, step_buf,
                       fonts_get_system_font(FONT_KEY_GOTHIC_14),
                       GRect(0, bounds.size.h - 20, bounds.size.w, 18),
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
  }
}

static void mark_dirty(void) {
  if (s_layer) {
    layer_mark_dirty(s_layer);
  }
  if (s_window) {
    window_set_background_color(s_window, bonsai_bg_color());
  }
}

void face_init(Window *window) {
  s_window = window;
  load_stages();

  Layer *root = window_get_root_layer(window);
  s_layer = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_layer, update_proc);
  layer_add_child(root, s_layer);

  face_tick();
  mark_dirty();
}

void face_deinit(void) {
  if (s_layer) {
    layer_destroy(s_layer);
    s_layer = NULL;
  }
  unload_stages();
  s_window = NULL;
}

void face_tick(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hour = t->tm_hour;
  s_minute = t->tm_min;
  s_wday = t->tm_wday;
  s_mday = t->tm_mday;
  s_mon = t->tm_mon;

  // Re-check steps each minute so midnight shed and growth stay current.
  bonsai_refresh();
  mark_dirty();
}

void face_pulse(void) {
  s_pulse = (s_pulse + 1) % 8;
  mark_dirty();
}

void face_apply_settings(void) {
  mark_dirty();
}

void face_on_bonsai_changed(void) {
  mark_dirty();
}
