#pragma once

#include <pebble.h>
#include <stdbool.h>

typedef enum {
  STAGE_BARE = 0,
  STAGE_BUDS,
  STAGE_SPARSE,
  STAGE_LEAFY,
  STAGE_LUSH,
  STAGE_BLOSSOM,
  STAGE_COUNT
} GrowthStage;

typedef struct {
  GrowthStage stage;
  int steps;
  int goal;
  bool has_steps;
  int day_of_year;  // for midnight shed detection
} BonsaiState;

typedef void (*BonsaiChangedFn)(void);

void bonsai_init(void);
void bonsai_deinit(void);
void bonsai_refresh(void);
void bonsai_on_settings_changed(void);
void bonsai_set_changed_callback(BonsaiChangedFn cb);
const BonsaiState *bonsai_get(void);

const char *bonsai_stage_label(GrowthStage stage);
uint32_t bonsai_stage_resource(GrowthStage stage);
GColor bonsai_bg_color(void);
GColor bonsai_fg_color(void);
