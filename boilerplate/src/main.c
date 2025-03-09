/**
 * Main Application File
 * 
 * This is the entry point for your EER application.
 */

#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include <stdio.h>
#include <unistd.h>

#include "components/my_component.h"

int main() {
    printf("Starting Application\n");

    // Create component instances
    eer_withprops(MyComponent, myComponent, _({.value = 1}));

    // Start the event loop
    loop(myComponent) {
        // Update the component value
        apply(MyComponent, myComponent,
              _({.value = myComponent.state.value + 1}));

        // Add your application logic here
        
        // Sleep to slow down the loop (remove in production)
        usleep(500000);  // 500ms

        // Exit condition
        if (myComponent.state.update_count >= 5) {
            printf("Application shutting down\n");
            eer_land.state.unmounted = true;
        }
    }

    printf("Application Completed\n");
    return 0;
}
