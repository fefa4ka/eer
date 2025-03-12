#!/bin/bash

# EER Project Generator
# This script generates a new EER project from the boilerplate

# Check if EER library is built
check_eer_library() {
    local eer_lib_path="$1/build/libeer.a"
    if [ ! -f "$eer_lib_path" ]; then
        echo "Error: EER library not found at $eer_lib_path"
        echo "Please build the EER framework first:"
        echo "  mkdir -p $1/build && cd $1/build && cmake .. && make"
        exit 1
    fi
}

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

# Check if EER library is built
check_eer_library "$(pwd)"

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
# Use gsed for macOS
SED="sed"
if [[ "$OSTYPE" == "darwin"* ]]; then
	SED="gsed"
fi

$SED -i "s/my_eer_project/$PROJECT_NAME/g" "$PROJECT_DIR/CMakeLists.txt"

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
    
    # Create component header file by copying the template and replacing the component name
    mkdir -p "$PROJECT_DIR/include/components"
    COMPONENT_NAME_LOWER=$(echo "$COMPONENT_NAME" | tr '[:upper:]' '[:lower:]')
    cp "boilerplate/include/components/my_component.h" "$PROJECT_DIR/include/components/$COMPONENT_NAME_LOWER.h"
    $SED -i "s/MyComponent/${COMPONENT_NAME}/g" "$PROJECT_DIR/include/components/$COMPONENT_NAME_LOWER.h"

    # Create component implementation file by copying the template and replacing the component name
    mkdir -p "$PROJECT_DIR/src/components"
    cp "boilerplate/src/components/my_component.c" "$PROJECT_DIR/src/components/$COMPONENT_NAME_LOWER.c"
    $SED -i "s/MyComponent/${COMPONENT_NAME}/g" "$PROJECT_DIR/src/components/$COMPONENT_NAME_LOWER.c"
    $SED -i "s/my_component/${COMPONENT_NAME_LOWER}/g" "$PROJECT_DIR/src/components/$COMPONENT_NAME_LOWER.c"

    # Create main.c by copying the template and replacing the component name
    cp "boilerplate/src/main.c" "$PROJECT_DIR/src/main.c"
    $SED -i "s/MyComponent/${COMPONENT_NAME}/g" "$PROJECT_DIR/src/main.c"
    $SED -i "s/my_component/${COMPONENT_NAME_LOWER}/g" "$PROJECT_DIR/src/main.c"
    $SED -i "s/myComponent/${COMPONENT_NAME,,}/g" "$PROJECT_DIR/src/main.c"
    $SED -i "s/Starting Application/Starting ${PROJECT_NAME} Application/g" "$PROJECT_DIR/src/main.c"

    # Create a test file for the component by copying the template and replacing the component name
    mkdir -p "$PROJECT_DIR/test"
    cp "boilerplate/test/my_component_test.c" "$PROJECT_DIR/test/${COMPONENT_NAME_LOWER}_test.c" 2>/dev/null || true
    if [ -f "$PROJECT_DIR/test/${COMPONENT_NAME_LOWER}_test.c" ]; then
        $SED -i "s/MyComponent/${COMPONENT_NAME}/g" "$PROJECT_DIR/test/${COMPONENT_NAME_LOWER}_test.c"
        $SED -i "s/my_component/${COMPONENT_NAME_LOWER}/g" "$PROJECT_DIR/test/${COMPONENT_NAME_LOWER}_test.c"
        $SED -i "s/testComponent/test${COMPONENT_NAME}/g" "$PROJECT_DIR/test/${COMPONENT_NAME_LOWER}_test.c"
    fi

    # Remove the default MyComponent files if we're creating a custom component
    rm -f "$PROJECT_DIR/include/components/my_component.h"
    rm -f "$PROJECT_DIR/src/components/my_component.c"
    rm -f "$PROJECT_DIR/test/my_component_test.c" 2>/dev/null
else
    # If no custom component is specified, rename the default component files
    # to match the project name for better clarity
    PROJECT_NAME_LOWER=$(echo "$PROJECT_NAME" | tr '[:upper:]' '[:lower:]')
    
    if [ -f "$PROJECT_DIR/include/components/my_component.h" ]; then
        mkdir -p "$PROJECT_DIR/include/components"
        $SED "s/MyComponent/${PROJECT_NAME}Component/g" "$PROJECT_DIR/include/components/my_component.h" > "$PROJECT_DIR/include/components/${PROJECT_NAME_LOWER}_component.h"
        rm "$PROJECT_DIR/include/components/my_component.h"
    fi
    
    if [ -f "$PROJECT_DIR/src/components/my_component.c" ]; then
        mkdir -p "$PROJECT_DIR/src/components"
        # First replace the component name
        $SED "s/MyComponent/${PROJECT_NAME}Component/g" "$PROJECT_DIR/src/components/my_component.c" > "$PROJECT_DIR/src/components/${PROJECT_NAME_LOWER}_component.c"
        # Then update the include path in the new file
        $SED -i "s|\"components/my_component.h\"|\"components/${PROJECT_NAME_LOWER}_component.h\"|g" "$PROJECT_DIR/src/components/${PROJECT_NAME_LOWER}_component.c"
        rm "$PROJECT_DIR/src/components/my_component.c"
    fi
    
    # Create main.c by copying the template and replacing the component name
    if [ -f "$PROJECT_DIR/src/main.c" ]; then
        $SED -i "s/my_component.h/${PROJECT_NAME_LOWER}_component.h/g" "$PROJECT_DIR/src/main.c"
        $SED -i "s/MyComponent/${PROJECT_NAME}Component/g" "$PROJECT_DIR/src/main.c"
        $SED -i "s/myComponent/${PROJECT_NAME_LOWER}Component/g" "$PROJECT_DIR/src/main.c"
        $SED -i "s/Starting Application/Starting ${PROJECT_NAME} Application/g" "$PROJECT_DIR/src/main.c"
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
