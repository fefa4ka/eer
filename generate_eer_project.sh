#!/bin/bash

# EER Project Generator
# This script generates a new EER project from the boilerplate

# Display help information
show_help() {
    echo "EER Project Generator"
    echo "Usage: $0 [options] project_name"
    echo ""
    echo "Options:"
    echo "  -h, --help                 Show this help message"
    echo "  -d, --directory DIR        Parent directory for the new project (default: current directory)"
    echo "  -c, --component NAME       Create an initial component with the specified name"
    echo "  -g, --git                  Initialize a git repository"
    echo "  -a, --author NAME          Specify the author name for documentation"
    echo "  -e, --email EMAIL          Specify the author email for documentation"
    echo ""
    echo "Example:"
    echo "  $0 --component Button --git MyProject"
    echo ""
}

# Default values
PARENT_DIR="."
INIT_GIT=false
COMPONENT_NAME=""
AUTHOR_NAME=$(git config --get user.name 2>/dev/null || echo "Your Name")
AUTHOR_EMAIL=$(git config --get user.email 2>/dev/null || echo "your.email@example.com")

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -d|--directory)
            PARENT_DIR="$2"
            shift 2
            ;;
        -c|--component)
            COMPONENT_NAME="$2"
            shift 2
            ;;
        -g|--git)
            INIT_GIT=true
            shift
            ;;
        -a|--author)
            AUTHOR_NAME="$2"
            shift 2
            ;;
        -e|--email)
            AUTHOR_EMAIL="$2"
            shift 2
            ;;
        -*)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
        *)
            PROJECT_NAME="$1"
            shift
            ;;
    esac
done

# Check if project name is provided
if [ -z "$PROJECT_NAME" ]; then
    echo "Error: Project name is required"
    show_help
    exit 1
fi

# Check if boilerplate directory exists
if [ ! -d "boilerplate" ]; then
    echo "Error: Boilerplate directory not found"
    echo "Make sure you run this script from the EER framework root directory"
    exit 1
fi

# Create project directory
PROJECT_DIR="$PARENT_DIR/$PROJECT_NAME"
if [ -d "$PROJECT_DIR" ]; then
    echo "Error: Directory $PROJECT_DIR already exists"
    exit 1
fi

echo "Creating new EER project: $PROJECT_NAME"
echo "Location: $PROJECT_DIR"

# Copy boilerplate directory
cp -r boilerplate "$PROJECT_DIR"

# Replace placeholder names in files
echo "Customizing project files..."

# Update CMakeLists.txt
sed -i "s/my_eer_project/$PROJECT_NAME/g" "$PROJECT_DIR/CMakeLists.txt"

# Create a README.md
cat > "$PROJECT_DIR/README.md" << EOF
# $PROJECT_NAME

A project built with the EER (Embedded Event-driven Reactor) framework.

## Description

This project uses the EER framework to create a reactive, component-based application.

## Getting Started

### Building the Project

\`\`\`bash
mkdir -p build && cd build
cmake ..
make
\`\`\`

### Running the Project

\`\`\`bash
./build/$PROJECT_NAME
\`\`\`

## Author

$AUTHOR_NAME <$AUTHOR_EMAIL>

## License

This project is licensed under the MIT License - see the LICENSE file for details.
EOF

# Create a custom component if specified
if [ ! -z "$COMPONENT_NAME" ]; then
    echo "Creating component: $COMPONENT_NAME"
    
    # Create component header file
    mkdir -p "$PROJECT_DIR/include/components"
    cat > "$PROJECT_DIR/include/components/${COMPONENT_NAME,,}.h" << EOF
/**
 * ${COMPONENT_NAME} Definition
 * 
 * This file defines a component with props and state.
 */

#pragma once

#include <eer.h>
#include <eer_comp.h>
#include <stdbool.h>
#include <stdio.h>

/* Define component props and state structures */
typedef struct {
    int value;
    // Add your props here
} ${COMPONENT_NAME}_props_t;

typedef struct {
    int value;
    int update_count;
    // Add your state here
} ${COMPONENT_NAME}_state_t;

/* Declare the component header */
eer_header(${COMPONENT_NAME});
EOF

    # Create component implementation file
    mkdir -p "$PROJECT_DIR/src/components"
    cat > "$PROJECT_DIR/src/components/${COMPONENT_NAME,,}.c" << EOF
/**
 * ${COMPONENT_NAME} Implementation
 * 
 * This file implements the lifecycle methods for ${COMPONENT_NAME}.
 */

#include "components/${COMPONENT_NAME,,}.h"

/* Implement lifecycle methods */

// Called when component is first mounted
WILL_MOUNT(${COMPONENT_NAME}) {
    // Initialize state based on props
    state->value = props->value;
    state->update_count = 0;
    printf("${COMPONENT_NAME} initialized with value: %d\\n", state->value);
}

// Determines if component should update
SHOULD_UPDATE(${COMPONENT_NAME}) {
    // Only update if value has changed
    return props->value != next_props->value;
}

// Called before update
WILL_UPDATE(${COMPONENT_NAME}) {
    printf("${COMPONENT_NAME} preparing to update from %d to %d\\n", 
           state->value, next_props->value);
}

// Main update function - apply props to state
RELEASE(${COMPONENT_NAME}) {
    state->value = props->value;
    state->update_count++;
}

// Called after update
DID_UPDATE(${COMPONENT_NAME}) {
    printf("${COMPONENT_NAME} updated to value: %d (update count: %d)\\n",
           state->value, state->update_count);
}

// Called after mount
DID_MOUNT(${COMPONENT_NAME}) {
    printf("${COMPONENT_NAME} mounted\\n");
}

// Called when unmounted
DID_UNMOUNT(${COMPONENT_NAME}) {
    printf("${COMPONENT_NAME} unmounted\\n");
}
EOF

    # Update main.c to use the new component
    cat > "$PROJECT_DIR/src/main.c" << EOF
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

#include "components/${COMPONENT_NAME,,}.h"

int main() {
    printf("Starting ${PROJECT_NAME} Application\\n");

    // Create component instances
    eer_withprops(${COMPONENT_NAME}, ${COMPONENT_NAME,,}, _({.value = 1}));

    // Start the event loop
    loop(${COMPONENT_NAME,,}) {
        // Update the component value
        apply(${COMPONENT_NAME}, ${COMPONENT_NAME,,},
              _({.value = ${COMPONENT_NAME,,}.state.value + 1}));

        // Add your application logic here
        
        // Sleep to slow down the loop (remove in production)
        usleep(500000);  // 500ms

        // Exit condition
        if (${COMPONENT_NAME,,}.state.update_count >= 5) {
            printf("Application shutting down\\n");
            eer_land.state.unmounted = true;
        }
    }

    printf("Application Completed\\n");
    return 0;
}
EOF

    # Create a test file for the component
    mkdir -p "$PROJECT_DIR/test"
    cat > "$PROJECT_DIR/test/${COMPONENT_NAME,,}_test.c" << EOF
/**
 * ${COMPONENT_NAME} Test
 * 
 * This file contains tests for the ${COMPONENT_NAME} component.
 */

#include <stdio.h>
#include <assert.h>
#include <eer.h>
#include <eer_app.h>
#include "components/${COMPONENT_NAME,,}.h"

int main() {
    printf("Running ${COMPONENT_NAME} tests\\n");
    
    // Create test component with initial value 10
    eer_withprops(${COMPONENT_NAME}, test${COMPONENT_NAME}, _({.value = 10}));
    
    // Test initial state
    assert(test${COMPONENT_NAME}.props.value == 10);
    
    // Start a test loop
    loop(test${COMPONENT_NAME}) {
        // Test updating the component
        apply(${COMPONENT_NAME}, test${COMPONENT_NAME}, _({.value = 20}));
        
        // Verify the update
        assert(test${COMPONENT_NAME}.props.value == 20);
        
        // Exit after verification
        break;
    }
    
    printf("All ${COMPONENT_NAME} tests passed!\\n");
    return 0;
}
EOF

    # Remove the default MyComponent files if we're creating a custom component
    rm -f "$PROJECT_DIR/include/components/my_component.h"
    rm -f "$PROJECT_DIR/src/components/my_component.c"
    rm -f "$PROJECT_DIR/test/my_component_test.c" 2>/dev/null
else
    # If no custom component is specified, rename the default component files
    # to match the project name for better clarity
    if [ -f "$PROJECT_DIR/include/components/my_component.h" ]; then
        mkdir -p "$PROJECT_DIR/include/components"
        sed "s/MyComponent/${PROJECT_NAME}Component/g" "$PROJECT_DIR/include/components/my_component.h" > "$PROJECT_DIR/include/components/${PROJECT_NAME,,}_component.h"
        rm "$PROJECT_DIR/include/components/my_component.h"
    fi
    
    if [ -f "$PROJECT_DIR/src/components/my_component.c" ]; then
        mkdir -p "$PROJECT_DIR/src/components"
        sed "s/MyComponent/${PROJECT_NAME}Component/g" "$PROJECT_DIR/src/components/my_component.c" > "$PROJECT_DIR/src/components/${PROJECT_NAME,,}_component.c"
        rm "$PROJECT_DIR/src/components/my_component.c"
    fi
    
    # Update main.c to use the renamed component
    if [ -f "$PROJECT_DIR/src/main.c" ]; then
        sed -i "s/my_component.h/${PROJECT_NAME,,}_component.h/g" "$PROJECT_DIR/src/main.c"
        sed -i "s/MyComponent/${PROJECT_NAME}Component/g" "$PROJECT_DIR/src/main.c"
        sed -i "s/myComponent/${PROJECT_NAME,,}Component/g" "$PROJECT_DIR/src/main.c"
    fi
fi

# Initialize git repository if requested
if [ "$INIT_GIT" = true ]; then
    echo "Initializing git repository..."
    cd "$PROJECT_DIR"
    git init
    git add .
    git commit -m "Initial commit: EER project created from boilerplate"
    cd - > /dev/null
fi

echo ""
echo "Project $PROJECT_NAME created successfully!"
echo ""
echo "Next steps:"
echo "1. cd $PROJECT_DIR"
echo "2. mkdir -p build && cd build"
echo "3. cmake .."
echo "4. make"
echo "5. ./$PROJECT_NAME"
echo ""
echo "Happy coding!"
