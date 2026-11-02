#pragma once

#include <pebble.h>
#include <stdbool.h>

typedef enum {
  TIME_FMT_SYSTEM = 0,
  TIME_FMT_12H,
  TIME_FMT_24H
} TimeFormat;

typedef enum {
  BG_ZEN = 0,
  BG_DAWN,
  BG_INK
} BgStyle;

typedef struct {
  TimeFormat time_format;
  bool leading_zero;
  bool show_date;
  bool show_steps;
  bool show_stage_label;
  bool demo_mode;
  int step_goal;
  BgStyle bg_style;
} FaceSettings;

typedef void (*SettingsChangedFn)(void);

void settings_init(void);
void settings_deinit(void);
void settings_set_changed_callback(SettingsChangedFn cb);
const FaceSettings *settings_get(void);
