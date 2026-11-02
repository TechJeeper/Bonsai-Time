#include "bonsai.h"
#include "settings.h"

static BonsaiState s_state;
static AppTimer *s_demo_timer;
static uint8_t s_demo_idx;
static BonsaiChangedFn s_changed;

static void notify_changed(void) {
  if (s_changed) {
    s_changed();
  }
}

static int day_of_year_now(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  return t->tm_yday + t->tm_year * 400;  // unique across years
}

static GrowthStage stage_from_progress(int steps, int goal) {
  if (goal <= 0) {
    goal = 8000;
  }
  if (steps <= 0) {
    return STAGE_BARE;
  }
  if (steps >= goal) {
    return STAGE_BLOSSOM;
  }

  // Map progress into the four leafy stages before blossom.
  int pct = (steps * 100) / goal;
  if (pct < 15) return STAGE_BUDS;
  if (pct < 35) return STAGE_SPARSE;
  if (pct < 60) return STAGE_LEAFY;
  return STAGE_LUSH;
}

static void apply_steps(int steps, bool has_steps) {
  GrowthStage prev_stage = s_state.stage;
  int prev_steps = s_state.steps;
  bool prev_has = s_state.has_steps;
  int day = day_of_year_now();

  // Midnight shed: when the calendar day flips, force bare until steps climb again.
  if (s_state.day_of_year != 0 && day != s_state.day_of_year) {
    s_state.stage = STAGE_BARE;
  }
  s_state.day_of_year = day;
  s_state.goal = settings_get()->step_goal;
  s_state.has_steps = has_steps;
  s_state.steps = has_steps ? steps : 0;

  if (has_steps) {
    s_state.stage = stage_from_progress(s_state.steps, s_state.goal);
  } else if (!settings_get()->demo_mode) {
    s_state.stage = STAGE_BARE;
  }

  if (prev_stage != s_state.stage || prev_steps != s_state.steps ||
      prev_has != s_state.has_steps) {
    notify_changed();
  }
}

#if defined(PBL_HEALTH)
static bool read_today_steps(int *out_steps) {
  HealthServiceAccessibilityMask mask =
      health_service_metric_accessible(HealthMetricStepCount, time_start_of_today(), time(NULL));
  if (!(mask & HealthServiceAccessibilityMaskAvailable)) {
    return false;
  }
  HealthValue value = health_service_sum_today(HealthMetricStepCount);
  if (value < 0) {
    return false;
  }
  *out_steps = (int)value;
  return true;
}

static void health_handler(HealthEventType event, void *context) {
  (void)context;
  if (event == HealthEventMovementUpdate ||
      event == HealthEventSignificantUpdate ||
      event == HealthEventMetricAlert) {
    bonsai_refresh();
  }
}
#endif

static void demo_tick(void *ctx) {
  (void)ctx;
  s_demo_timer = NULL;
  if (!settings_get()->demo_mode) {
    return;
  }
  if (s_state.has_steps && s_state.steps > 0) {
    // Live steps win over demo cycling.
    s_demo_timer = app_timer_register(3500, demo_tick, NULL);
    return;
  }
  s_demo_idx = (uint8_t)((s_demo_idx + 1) % STAGE_COUNT);
  s_state.stage = (GrowthStage)s_demo_idx;
  s_state.goal = settings_get()->step_goal;
  static const int demo_steps[STAGE_COUNT] = {0, 800, 2400, 4500, 6500, 8000};
  s_state.steps = demo_steps[s_demo_idx];
  s_state.has_steps = false;
  notify_changed();
  s_demo_timer = app_timer_register(3500, demo_tick, NULL);
}

void bonsai_set_changed_callback(BonsaiChangedFn cb) {
  s_changed = cb;
}

void bonsai_on_settings_changed(void) {
  if (s_demo_timer) {
    app_timer_cancel(s_demo_timer);
    s_demo_timer = NULL;
  }
  bonsai_refresh();
  if (settings_get()->demo_mode) {
    s_demo_timer = app_timer_register(3500, demo_tick, NULL);
  }
  notify_changed();
}

void bonsai_init(void) {
  s_state.stage = STAGE_BARE;
  s_state.steps = 0;
  s_state.goal = settings_get()->step_goal;
  s_state.has_steps = false;
  s_state.day_of_year = day_of_year_now();
  s_demo_idx = 0;

#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
#endif
  bonsai_refresh();

  if (settings_get()->demo_mode) {
    s_demo_timer = app_timer_register(3500, demo_tick, NULL);
  }
}

void bonsai_deinit(void) {
  if (s_demo_timer) {
    app_timer_cancel(s_demo_timer);
    s_demo_timer = NULL;
  }
#if defined(PBL_HEALTH)
  health_service_events_unsubscribe();
#endif
}

void bonsai_refresh(void) {
  int steps = 0;
  bool has_steps = false;

#if defined(PBL_HEALTH)
  has_steps = read_today_steps(&steps);
#endif

  if (has_steps) {
    apply_steps(steps, true);
  } else if (settings_get()->demo_mode) {
    if (s_state.steps <= 0 && s_state.stage == STAGE_BARE) {
      // Seed demo so the first frame isn't empty forever.
      s_state.stage = STAGE_BARE;
      s_state.steps = 0;
    }
    s_state.goal = settings_get()->step_goal;
    s_state.has_steps = false;
    s_state.day_of_year = day_of_year_now();
  } else {
    apply_steps(0, false);
  }
}

const BonsaiState *bonsai_get(void) {
  return &s_state;
}

const char *bonsai_stage_label(GrowthStage stage) {
  switch (stage) {
    case STAGE_BARE:    return "BARE";
    case STAGE_BUDS:    return "BUDS";
    case STAGE_SPARSE:  return "SPARSE";
    case STAGE_LEAFY:   return "LEAFY";
    case STAGE_LUSH:    return "LUSH";
    case STAGE_BLOSSOM: return "BLOSSOM";
    default:            return "BARE";
  }
}

uint32_t bonsai_stage_resource(GrowthStage stage) {
  switch (stage) {
    case STAGE_BARE:    return RESOURCE_ID_IMAGE_BONSAI_BARE;
    case STAGE_BUDS:    return RESOURCE_ID_IMAGE_BONSAI_BUDS;
    case STAGE_SPARSE:  return RESOURCE_ID_IMAGE_BONSAI_SPARSE;
    case STAGE_LEAFY:   return RESOURCE_ID_IMAGE_BONSAI_LEAFY;
    case STAGE_LUSH:    return RESOURCE_ID_IMAGE_BONSAI_LUSH;
    case STAGE_BLOSSOM: return RESOURCE_ID_IMAGE_BONSAI_BLOSSOM;
    default:            return RESOURCE_ID_IMAGE_BONSAI_BARE;
  }
}

GColor bonsai_bg_color(void) {
#if defined(PBL_COLOR)
  switch (settings_get()->bg_style) {
    case BG_DAWN: return GColorPictonBlue;
    case BG_INK:  return GColorBlack;
    case BG_ZEN:
    default:      return GColorBrass;
  }
#else
  return GColorBlack;
#endif
}

GColor bonsai_fg_color(void) {
#if defined(PBL_COLOR)
  switch (settings_get()->bg_style) {
    case BG_DAWN: return GColorBlack;
    case BG_INK:  return GColorWhite;
    case BG_ZEN:
    default:      return GColorBlack;
  }
#else
  return GColorWhite;
#endif
}
