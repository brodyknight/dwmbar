#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  time_t last_update;
  int refresh_interval;
} Timer;

void setstatus(const char *str) {
  Display *d;
  Window root;

  d = XOpenDisplay(NULL);
  if (d == NULL) {
    fprintf(stderr, "Cannot open display\n");
    return;
  }

  root = DefaultRootWindow(d);
  XStoreName(d, root, str);
  XCloseDisplay(d);
}

int update_check(Timer *timer) {
  time_t now = time(NULL);
  if (difftime(now, timer->last_update) >= timer->refresh_interval) {
    timer->last_update = now;
    return 1;
  }
  return 0;
}

void getbattery(const char *bat, char *output, size_t output_size) {
  char cap_path[256];
  char stat_path[256];
  FILE *cp;
  FILE *sp;
  int capacity;
  char status[64];
  const char *charging_icon = "⚡";

  // Construct the full path
  snprintf(cap_path, sizeof(cap_path), "/sys/class/power_supply/%s/capacity", bat);
  snprintf(stat_path, sizeof(stat_path), "/sys/class/power_supply/%s/status", bat);

  cp = fopen(cap_path, "r");
  if (cp == NULL) {
    perror("Failed to open battery capacity");
    snprintf(output, output_size, "BAT: Error");
    return;
  }

  sp = fopen(stat_path, "r");
  if (sp == NULL) {
    perror("Failed to open battery status");
    snprintf(output, output_size, "BAT: Error");
    return;
  }

  if (fscanf(cp, "%d", &capacity) != 1) {
    perror("Failed to read battery capacity");
    fclose(cp);
    fclose(sp);
    snprintf(output, output_size, "BAT: Error");
    return;
  }

  if (fscanf(sp, "%s", status) != 1) {
    perror("Failed to read battery status");
    fclose(cp);
    fclose(sp);
    snprintf(output, output_size, "BAT: Error");
    return;
  }

  fclose(cp);
  fclose(sp);

  if (strcmp(status, "Charging") == 0) {
    snprintf(output, output_size, "%s %d%%", charging_icon, capacity);
  } else {
    snprintf(output, output_size, "BAT: %d%%", capacity);
  }
}

void gettime(char *buffer, size_t buflen) {
  time_t s;
  struct tm *current_time;

  // Get current time
  s = time(NULL);

  // Convert to local time format
  current_time = localtime(&s);

  // Format the time into buffer
  snprintf(buffer, buflen, "%02d:%02d", current_time->tm_hour,
           current_time->tm_min);
}

int main(void) {
  Timer battery_timer = {0, 10};

  char battery_status[64];
  char time_str[9] = "00:00:00";
  char status[256];

  while (1) {
    if (update_check(&battery_timer)) {
      getbattery("BAT0", battery_status, sizeof(battery_status));
    }

    gettime(time_str, sizeof(time_str));

    snprintf(status, sizeof(status), "%s | %s", battery_status, time_str);

    // Update status bar
    setstatus(status);

    sleep(1);
  }

  return 0;
}