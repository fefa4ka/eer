/**
 * MyComponent Test
 * 
 * This file contains tests for MyComponent.
 */

#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include <stdio.h>
#include <assert.h>

#include "components/my_component.h"

// Create a test instance of MyComponent
eer_withprops(MyComponent, testComponent, _({.value = 10}));

int main() {
    printf("Running MyComponent tests\n");
    
    // Test initial state
    assert(testComponent.props.value == 10);
    
    // Start a test loop
    loop(testComponent) {
        // Test updating the component
        apply(MyComponent, testComponent, _({.value = 20}));
        
        // Verify the update
        assert(testComponent.state.value == 20);
        assert(testComponent.state.update_count == 1);
        
        // Exit after verification
        eer_land.state.unmounted = true;
    }
    
    printf("All tests passed!\n");
    return 0;
}
