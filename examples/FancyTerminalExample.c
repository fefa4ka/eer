/**
 * Fancy Terminal Example
 *
 * This example demonstrates a more complex application with the EER Framework.
 * It creates a terminal-based UI with multiple components:
 * - ClockComponent: Displays the current time
 * - AnimationComponent: Shows an animated ASCII art
 * - MenuComponent: Provides an interactive menu
 * - StatusComponent: Shows system status information
 */

#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Terminal utilities
static struct termios orig_termios;

void clear_screen() {
  printf("\033[2J\033[H"); // Clear screen and move cursor to top-left
  fflush(stdout);
}

void move_cursor(int row, int col) {
  printf("\033[%d;%dH", row, col);
  fflush(stdout);
}

void set_color(int fg, int bg) {
  printf("\033[%d;%dm", fg, bg);
  fflush(stdout);
}

void reset_color() {
  printf("\033[0m");
  fflush(stdout);
}

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VMIN] = 0;  // Return immediately with available chars
  raw.c_cc[VTIME] = 0; // No timeout
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() { 
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); 
}

int get_terminal_width() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}

int get_terminal_height() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_row;
}

char read_key() {
  char c = 0;
  read(STDIN_FILENO, &c, 1);
  return c;
}

// Clock Component
typedef struct {
  bool show_seconds;
} ClockComponent_props_t;

typedef struct {
  bool show_seconds;
  char time_str[20];
  int updates;
} ClockComponent_state_t;

eer_header(ClockComponent);

WILL_MOUNT(ClockComponent) {
  state->show_seconds = props->show_seconds;
  state->updates = 0;
  strcpy(state->time_str, "00:00:00");
}

SHOULD_UPDATE_SKIP(ClockComponent);
WILL_UPDATE_SKIP(ClockComponent);

RELEASE(ClockComponent) {
  state->show_seconds = props->show_seconds;
  
  // Get current time
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  
  if (state->show_seconds) {
    strftime(state->time_str, sizeof(state->time_str), "%H:%M:%S", tm_info);
  } else {
    strftime(state->time_str, sizeof(state->time_str), "%H:%M", tm_info);
  }
  
  state->updates++;
}

DID_UPDATE(ClockComponent) {
  int term_width = get_terminal_width();
  move_cursor(1, term_width - strlen(state->time_str) - 2);
  set_color(36, 40); // Cyan text
  printf("%s", state->time_str);
  reset_color();
}

DID_MOUNT(ClockComponent) {
  // Initial render
  int term_width = get_terminal_width();
  move_cursor(1, term_width - 10);
  set_color(36, 40); // Cyan text
  printf("%s", state->time_str);
  reset_color();
}

DID_UNMOUNT_SKIP(ClockComponent);

// Animation Component
typedef struct {
  int speed;
  bool enabled;
} AnimationComponent_props_t;

typedef struct {
  int speed;
  bool enabled;
  int frame;
  int max_frames;
  char frames[5][80];
} AnimationComponent_state_t;

eer_header(AnimationComponent);

WILL_MOUNT(AnimationComponent) {
  state->speed = props->speed;
  state->enabled = props->enabled;
  state->frame = 0;
  state->max_frames = 5;
  
  // Define animation frames
  strcpy(state->frames[0], "    *     *     *     *     *     *     *    ");
  strcpy(state->frames[1], "   ***   ***   ***   ***   ***   ***   ***   ");
  strcpy(state->frames[2], "  ***** ***** ***** ***** ***** ***** *****  ");
  strcpy(state->frames[3], " *************************************** ");
  strcpy(state->frames[4], "***************************************** ");
}

SHOULD_UPDATE(AnimationComponent) {
  return props->enabled != next_props->enabled || 
         props->speed != next_props->speed ||
         state->enabled;
}

WILL_UPDATE_SKIP(AnimationComponent);

RELEASE(AnimationComponent) {
  state->speed = props->speed;
  state->enabled = props->enabled;
  
  if (state->enabled) {
    // Update animation frame
    state->frame = (state->frame + 1) % state->max_frames;
  }
}

DID_UPDATE(AnimationComponent) {
  if (state->enabled) {
    int term_width = get_terminal_width();
    int padding = (term_width - strlen(state->frames[0])) / 2;
    
    move_cursor(3, padding);
    set_color(33, 40); // Yellow text
    printf("%s", state->frames[state->frame]);
    reset_color();
  }
}

DID_MOUNT(AnimationComponent) {
  // Initial render
  int term_width = get_terminal_width();
  int padding = (term_width - strlen(state->frames[0])) / 2;
  
  move_cursor(3, padding);
  set_color(33, 40); // Yellow text
  printf("%s", state->frames[state->frame]);
  reset_color();
}

DID_UNMOUNT_SKIP(AnimationComponent);

// Menu Component
typedef struct {
  char key;
} MenuComponent_props_t;

typedef struct {
  char key;
  int selected_item;
  char menu_items[4][20];
  char status_message[100];
} MenuComponent_state_t;

eer_header(MenuComponent);

WILL_MOUNT(MenuComponent) {
  state->key = props->key;
  state->selected_item = 0;
  
  // Define menu items
  strcpy(state->menu_items[0], "1. Toggle Animation");
  strcpy(state->menu_items[1], "2. Toggle Seconds");
  strcpy(state->menu_items[2], "3. Change Speed");
  strcpy(state->menu_items[3], "q. Quit");
  
  strcpy(state->status_message, "Welcome to Fancy Terminal Example!");
}

SHOULD_UPDATE(MenuComponent) {
  return props->key != next_props->key && next_props->key != 0;
}

WILL_UPDATE_SKIP(MenuComponent);

RELEASE(MenuComponent) {
  state->key = props->key;
  
  // Process menu selection
  switch (state->key) {
    case '1':
      strcpy(state->status_message, "Animation toggled");
      break;
    case '2':
      strcpy(state->status_message, "Seconds display toggled");
      break;
    case '3':
      strcpy(state->status_message, "Animation speed changed");
      break;
    case 'q':
      strcpy(state->status_message, "Exiting application...");
      break;
    default:
      snprintf(state->status_message, sizeof(state->status_message), 
               "Unknown command: %c", state->key);
      break;
  }
}

DID_UPDATE(MenuComponent) {
  // Render menu
  int term_height = get_terminal_height();
  int menu_start_row = term_height - 8;
  
  // Draw menu border
  move_cursor(menu_start_row - 1, 2);
  set_color(37, 44); // White on blue
  for (int i = 0; i < 40; i++) printf(" ");
  reset_color();
  
  // Draw menu items
  for (int i = 0; i < 4; i++) {
    move_cursor(menu_start_row + i, 4);
    if (i == state->selected_item) {
      set_color(30, 47); // Black on white (highlighted)
    } else {
      set_color(37, 40); // White on black
    }
    printf("%s", state->menu_items[i]);
    reset_color();
  }
  
  // Draw status message
  move_cursor(menu_start_row + 5, 4);
  set_color(32, 40); // Green text
  printf("Status: %s", state->status_message);
  reset_color();
}

DID_MOUNT(MenuComponent) {
  // Initial render
  int term_height = get_terminal_height();
  int menu_start_row = term_height - 8;
  
  // Draw menu border
  move_cursor(menu_start_row - 1, 2);
  set_color(37, 44); // White on blue
  for (int i = 0; i < 40; i++) printf(" ");
  reset_color();
  
  // Draw menu items
  for (int i = 0; i < 4; i++) {
    move_cursor(menu_start_row + i, 4);
    if (i == state->selected_item) {
      set_color(30, 47); // Black on white (highlighted)
    } else {
      set_color(37, 40); // White on black
    }
    printf("%s", state->menu_items[i]);
    reset_color();
  }
  
  // Draw status message
  move_cursor(menu_start_row + 5, 4);
  set_color(32, 40); // Green text
  printf("Status: %s", state->status_message);
  reset_color();
}

DID_UNMOUNT_SKIP(MenuComponent);

// Status Component
typedef struct {
  int animation_speed;
  bool animation_enabled;
  bool show_seconds;
} StatusComponent_props_t;

typedef struct {
  int animation_speed;
  bool animation_enabled;
  bool show_seconds;
  int update_count;
} StatusComponent_state_t;

eer_header(StatusComponent);

WILL_MOUNT(StatusComponent) {
  state->animation_speed = props->animation_speed;
  state->animation_enabled = props->animation_enabled;
  state->show_seconds = props->show_seconds;
  state->update_count = 0;
}

SHOULD_UPDATE(StatusComponent) {
  return props->animation_speed != next_props->animation_speed ||
         props->animation_enabled != next_props->animation_enabled ||
         props->show_seconds != next_props->show_seconds;
}

WILL_UPDATE_SKIP(StatusComponent);

RELEASE(StatusComponent) {
  state->animation_speed = props->animation_speed;
  state->animation_enabled = props->animation_enabled;
  state->show_seconds = props->show_seconds;
  state->update_count++;
}

DID_UPDATE(StatusComponent) {
  int term_height = get_terminal_height();
  int status_row = term_height - 2;
  
  move_cursor(status_row, 2);
  set_color(35, 40); // Magenta text
  printf("Animation: %s | Speed: %d | Seconds: %s | Updates: %d",
         state->animation_enabled ? "ON " : "OFF",
         state->animation_speed,
         state->show_seconds ? "ON " : "OFF",
         state->update_count);
  reset_color();
}

DID_MOUNT(StatusComponent) {
  int term_height = get_terminal_height();
  int status_row = term_height - 2;
  
  move_cursor(status_row, 2);
  set_color(35, 40); // Magenta text
  printf("Animation: %s | Speed: %d | Seconds: %s | Updates: %d",
         state->animation_enabled ? "ON " : "OFF",
         state->animation_speed,
         state->show_seconds ? "ON " : "OFF",
         state->update_count);
  reset_color();
}

DID_UNMOUNT_SKIP(StatusComponent);

// Create component instances
eer_withprops(ClockComponent, clockComponent, _({.show_seconds = true}));
eer_withprops(AnimationComponent, animationComponent, _({.speed = 1, .enabled = true}));
eer_withprops(MenuComponent, menuComponent, _({.key = 0}));
eer_withprops(StatusComponent, statusComponent, 
              _({.animation_speed = 1, 
                 .animation_enabled = true, 
                 .show_seconds = true}));

int main() {
  // Initialize terminal
  enable_raw_mode();
  clear_screen();
  
  // Draw application title
  int term_width = get_terminal_width();
  int title_padding = (term_width - 24) / 2;
  move_cursor(1, title_padding);
  set_color(37, 44); // White on blue
  printf(" Fancy Terminal Example ");
  reset_color();
  
  // Start the event loop
  loop(clockComponent, animationComponent, menuComponent, statusComponent) {
    // Check for keyboard input
    char key = read_key();
    if (key != 0) {
      apply(MenuComponent, menuComponent, _({.key = key}));
      
      // Process key commands
      switch (key) {
        case '1':
          // Toggle animation
          apply(AnimationComponent, animationComponent, 
                _({.speed = animationComponent.state.speed, 
                   .enabled = !animationComponent.state.enabled}));
          apply(StatusComponent, statusComponent,
                _({.animation_speed = animationComponent.state.speed,
                   .animation_enabled = !animationComponent.state.enabled,
                   .show_seconds = clockComponent.state.show_seconds}));
          break;
        case '2':
          // Toggle seconds display
          apply(ClockComponent, clockComponent, 
                _({.show_seconds = !clockComponent.state.show_seconds}));
          apply(StatusComponent, statusComponent,
                _({.animation_speed = animationComponent.state.speed,
                   .animation_enabled = animationComponent.state.enabled,
                   .show_seconds = !clockComponent.state.show_seconds}));
          break;
        case '3':
          // Change animation speed
          {
            int new_speed = (animationComponent.state.speed % 3) + 1;
            apply(AnimationComponent, animationComponent, 
                  _({.speed = new_speed, 
                     .enabled = animationComponent.state.enabled}));
            apply(StatusComponent, statusComponent,
                  _({.animation_speed = new_speed,
                     .animation_enabled = animationComponent.state.enabled,
                     .show_seconds = clockComponent.state.show_seconds}));
          }
          break;
        case 'q':
          // Exit application
          eer_land.state.unmounted = true;
          break;
      }
    }
    
    // Small delay to prevent CPU hogging
    usleep(100000 / animationComponent.state.speed); // Adjust delay based on animation speed
  }
  
  // Clean up
  clear_screen();
  move_cursor(1, 1);
  disable_raw_mode();
  printf("Fancy Terminal Example Completed\n");
  
  return 0;
}
