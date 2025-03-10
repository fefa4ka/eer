/**
 * Keyboard Input Example
 *
 * This example demonstrates handling keyboard input with the EER Framework.
 * It uses two components:
 * - KeyboardComponent: Captures keyboard input
 * - CommandComponent: Processes commands based on keyboard input
 */

#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// Utility function to enable raw terminal mode for keyboard input
static struct termios orig_termios;

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Define the Keyboard component
typedef struct {
  bool enabled;
} KeyboardComponent_props_t;

typedef struct {
  bool enabled;
  char key;
  bool has_input;
} KeyboardComponent_state_t;

eer_header(KeyboardComponent);

// Lifecycle methods for KeyboardComponent
WILL_MOUNT(KeyboardComponent) {
  state->enabled = props->enabled;
  state->key = 0;
  state->has_input = false;
  printf("Keyboard component initialized\n");
}

SHOULD_UPDATE(KeyboardComponent) {
  return props->enabled != next_props->enabled;
}

WILL_UPDATE_SKIP(KeyboardComponent);

RELEASE(KeyboardComponent) {
  state->enabled = props->enabled;
  
  // Read keyboard input if enabled
  if (state->enabled) {
    char c = 0;
    if (read(STDIN_FILENO, &c, 1) == 1) {
      state->key = c;
      state->has_input = true;
    } else {
      state->has_input = false;
    }
  }
}

DID_UPDATE(KeyboardComponent) {
  if (state->has_input) {
    printf("Key pressed: %c\n", state->key);
  }
}

DID_MOUNT(KeyboardComponent) {
  printf("Keyboard component mounted\n");
  enable_raw_mode();
}

DID_UNMOUNT(KeyboardComponent) {
  printf("Keyboard component unmounted\n");
  disable_raw_mode();
}

// Create a keyboard component instance
eer_withprops(KeyboardComponent, keyboard, _({.enabled = true}));

// Define the Command component
typedef struct {
  char key;
} CommandComponent_props_t;

typedef struct {
  char key;
  char response[100];
  int command_count;
} CommandComponent_state_t;

eer_header(CommandComponent);

// Lifecycle methods for CommandComponent
WILL_MOUNT(CommandComponent) {
  state->key = props->key;
  state->command_count = 0;
  strcpy(state->response, "No command yet");
  printf("Command component initialized\n");
}

SHOULD_UPDATE(CommandComponent) {
  return props->key != next_props->key && next_props->key != 0;
}

WILL_UPDATE_SKIP(CommandComponent);

RELEASE(CommandComponent) {
  state->key = props->key;
  state->command_count++;
  
  // Process different commands based on key input
  switch (state->key) {
    case 'h':
      strcpy(state->response, "Hello, world!");
      break;
    case 'q':
      strcpy(state->response, "Quitting application...");
      break;
    case '1':
      strcpy(state->response, "Command One executed");
      break;
    case '2':
      strcpy(state->response, "Command Two executed");
      break;
    case '?':
      strcpy(state->response, "Help: Press h, q, 1, 2, or ? for different commands");
      break;
    default:
      snprintf(state->response, sizeof(state->response), 
               "Unknown command: %c", state->key);
      break;
  }
}

DID_UPDATE(CommandComponent) {
  printf("Command response: %s (command #%d)\n", 
         state->response, state->command_count);
}

DID_MOUNT(CommandComponent) {
  printf("Command component mounted\n");
  printf("Type '?' for help\n");
}

DID_UNMOUNT(CommandComponent) {
  printf("Command component unmounted\n");
}

// Create a command component instance
eer_withprops(CommandComponent, command, _({.key = 0}));

int main() {
  printf("Starting Keyboard Example\n");
  printf("Press keys to issue commands (q to quit)\n");

  // Start the event loop
  loop() {
    // Use the keyboard component to get input
    with(keyboard) {
      // Process the keyboard input with the command component
      if (keyboard.state.has_input) {
        apply(CommandComponent, command, _({.key = keyboard.state.key}));
        
        // Reset the keyboard input flag
        keyboard.state.has_input = false;
        
        // Exit if 'q' is pressed
        if (keyboard.state.key == 'q') {
          printf("Exiting application\n");
          eer_land.state.unmounted = true;
        }
      }
    }
    
    // Small delay to prevent CPU hogging
    usleep(10000); // 10ms delay
  }

  printf("Keyboard Example Completed\n");
  return 0;
}
