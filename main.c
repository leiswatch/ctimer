/* Copyright(c) 2025 leiswatch. All Rights Reserved. */

#include <math.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 300
#define FPS 60
#define TEXT_SCALE 0.21
#define SPACING 10.0
#define TIMER_COUNT 8
#define MAX_TIME 60 * 60 * 99
#define ZERO "0"
#define SEMI ":"

typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} timer_data_t;

typedef struct {
  Font font;
  timer_data_t* timer;
  float text_size;
  float window_width;
  float window_height;
  float currentTime;
} render_timer_t;

typedef enum {
  MODE_ASCENDING,
  MODE_COUNTDOWN,
} Mode;

static const size_t t = sizeof("%d");

float parse_time(const char* time) {
  float result = 0.0;

  while (*time) {
    char* endptr = NULL;
    float x = strtof(time, &endptr);

    if (time == endptr) {
      fprintf(stderr, "`%s` is not a number\n", time);
      exit(EXIT_FAILURE);
    }

    switch (*endptr) {
      case '\0':
      case 's':
        result += x;
        break;
      case 'm':
        result += x * 60.0;
        break;
      case 'h':
        result += x * 60.0 * 60.0;
        break;
      default:
        fprintf(stderr, "`%c` is an unknown time unit\n", *endptr);
        exit(EXIT_FAILURE);
    }

    time = endptr;

    if (*time) {
      time += 1;
    }
  }

  if (result > MAX_TIME) {
    fprintf(stderr, "You can't pass more than 99 hours\n");
    exit(EXIT_FAILURE);
  }

  return result;
}

void render_timer(const render_timer_t* render_obj) {
  Vector2 zero_measurement =
      MeasureTextEx(render_obj->font, ZERO, render_obj->text_size, SPACING);
  Vector2 semi_measurement =
      MeasureTextEx(render_obj->font, SEMI, render_obj->text_size, SPACING);

  float timer_width = (zero_measurement.x * 6 + semi_measurement.x * 2 +
                       SPACING * (TIMER_COUNT - 1));
  float mid = (render_obj->window_width - timer_width) * 0.5;
  float char_width = zero_measurement.x + SPACING;

  char part[3];
  Vector2 char_pos = {
      .x = 0,
      .y = (render_obj->window_height - zero_measurement.y) * 0.5,
  };

  for (int i = 0; i < TIMER_COUNT; ++i) {
    float y = char_pos.y;
    float x = mid + i * char_width;

    if (i > 5) {
      x = mid + i * char_width - zero_measurement.x * 2 +
          semi_measurement.x * 2;
    } else if (i > 2) {
      x = mid + i * char_width - zero_measurement.x + semi_measurement.x;
    }

    y = char_pos.y + sinf(render_obj->currentTime + i) * 10.0;

    char_pos.x = x;
    char_pos.y = y;

    if (i == 6) {
      snprintf(part, t, "%d", render_obj->timer->seconds / 10);
    } else if (i == 7) {
      snprintf(part, t, "%d", render_obj->timer->seconds % 10);
    } else if (i == 3) {
      snprintf(part, t, "%d", render_obj->timer->minutes / 10);
    } else if (i == 4) {
      snprintf(part, t, "%d", render_obj->timer->minutes % 10);
    } else if (i == 0) {
      snprintf(part, t, "%d", render_obj->timer->hours / 10);
    } else if (i == 1) {
      snprintf(part, t, "%d", render_obj->timer->hours % 10);
    } else {
      snprintf(part, t, ":");
    }

    DrawText(part, char_pos.x, y, render_obj->text_size, LIGHTGRAY);
  }
}

void set_time(timer_data_t* timer, uint32_t total_seconds) {
  uint8_t seconds = total_seconds % 60;
  uint8_t minutes = (int)floorf(total_seconds / 60.0) % 60;
  uint8_t hours = (int)floorf(total_seconds / 3600.0);

  timer->seconds = seconds;
  timer->minutes = minutes;
  timer->hours = hours;
}

int main(int argc, char* argv[]) {
  Mode mode = MODE_ASCENDING;
  uint32_t seconds = 0;

  if (argc > 1) {
    mode = MODE_COUNTDOWN;
    seconds = (uint32_t)parse_time(argv[1]);
  }

  SetTraceLogLevel(LOG_NONE);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ctimer");
  InitAudioDevice();
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(FPS);
  Sound sound = LoadSound("./notification.wav");

  const Font default_font = GetFontDefault();

  timer_data_t timer = {0};
  render_timer_t render_obj = {
      .text_size = WINDOW_WIDTH * TEXT_SCALE,
      .window_width = WINDOW_WIDTH,
      .window_height = WINDOW_HEIGHT,
      .timer = &timer,
      .font = default_font,
      .currentTime = 0.0,
  };

  double initialTime = 0.0;
  double currentTime = 0.0;
  int render_width = WINDOW_WIDTH;
  int render_height = WINDOW_HEIGHT;

  set_time(&timer, seconds);

  while (!WindowShouldClose()) {
    currentTime = GetTime();
    render_obj.currentTime = fmodf(currentTime, PI * 2);

    if (currentTime - initialTime >= 1) {
      switch (mode) {
        case MODE_COUNTDOWN:
          seconds--;
          break;
        case MODE_ASCENDING:
          seconds++;
          break;
        default:
          break;
      }

      set_time(&timer, seconds);
      initialTime = currentTime;
    }

    if (IsWindowResized()) {
      render_width = GetRenderWidth();
      render_height = GetRenderHeight();
      render_obj.window_width = render_width;
      render_obj.window_height = render_height;
      render_obj.text_size = render_width * TEXT_SCALE;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    render_timer(&render_obj);
    EndDrawing();

    switch (mode) {
      case MODE_COUNTDOWN:
        if (seconds == 0) {
          PlaySound(sound);
          sleep(1);
          goto endProgram;
        }
        break;
      case MODE_ASCENDING:
        if (seconds > MAX_TIME) {  // support only 99hrs
          PlaySound(sound);
          sleep(1);
          goto endProgram;
        }
        break;
      default:
        break;
    }
  }

endProgram:
  UnloadSound(sound);
  UnloadFont(default_font);
  CloseAudioDevice();
  CloseWindow();

  return EXIT_SUCCESS;
}
