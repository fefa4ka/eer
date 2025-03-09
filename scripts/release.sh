#!/bin/bash
# EER Framework Release Script
# Usage: ./scripts/release.sh <major|minor|patch>

set -e

# Check if the working directory is clean
if [[ -n $(git status --porcelain) ]]; then
  echo "Error: Working directory is not clean. Commit or stash changes first."
  exit 1
fi

# Check argument
if [[ $# -ne 1 || ($1 != "major" && $1 != "minor" && $1 != "patch") ]]; then
  echo "Usage: $0 <major|minor|patch>"
  exit 1
fi

# Get current version from CMakeLists.txt
MAJOR=$(grep "set(EER_VERSION_MAJOR" CMakeLists.txt | gsed 's/[^0-9]*//g')
MINOR=$(grep "set(EER_VERSION_MINOR" CMakeLists.txt | gsed 's/[^0-9]*//g')
PATCH=$(grep "set(EER_VERSION_PATCH" CMakeLists.txt | gsed 's/[^0-9]*//g')

echo "Current version: $MAJOR.$MINOR.$PATCH"

# Calculate new version
if [[ $1 == "major" ]]; then
  NEW_MAJOR=$((MAJOR + 1))
  NEW_MINOR=0
  NEW_PATCH=0
elif [[ $1 == "minor" ]]; then
  NEW_MAJOR=$MAJOR
  NEW_MINOR=$((MINOR + 1))
  NEW_PATCH=0
else
  NEW_MAJOR=$MAJOR
  NEW_MINOR=$MINOR
  NEW_PATCH=$((PATCH + 1))
fi

NEW_VERSION="$NEW_MAJOR.$NEW_MINOR.$NEW_PATCH"
echo "New version: $NEW_VERSION"

# Update version in CMakeLists.txt
gsed -i "s/set(EER_VERSION_MAJOR $MAJOR)/set(EER_VERSION_MAJOR $NEW_MAJOR)/" CMakeLists.txt
gsed -i "s/set(EER_VERSION_MINOR $MINOR)/set(EER_VERSION_MINOR $NEW_MINOR)/" CMakeLists.txt
gsed -i "s/set(EER_VERSION_PATCH $PATCH)/set(EER_VERSION_PATCH $NEW_PATCH)/" CMakeLists.txt

# Update version in include/eer.h
gsed -i "s/#define EER_VERSION_MAJOR $MAJOR/#define EER_VERSION_MAJOR $NEW_MAJOR/" include/eer.h
gsed -i "s/#define EER_VERSION_MINOR $MINOR/#define EER_VERSION_MINOR $NEW_MINOR/" include/eer.h
gsed -i "s/#define EER_VERSION_PATCH $PATCH/#define EER_VERSION_PATCH $NEW_PATCH/" include/eer.h
gsed -i "s/#define EER_VERSION_STRING \"$MAJOR.$MINOR.$PATCH\"/#define EER_VERSION_STRING \"$NEW_VERSION\"/" include/eer.h
gsed -i "s/@version $MAJOR.$MINOR.$PATCH/@version $NEW_VERSION/" include/eer.h

# Update CHANGELOG.md - add new version section
DATE=$(date +%Y-%m-%d)
gsed -i "s/## \[$MAJOR.$MINOR.$PATCH\]/## [$NEW_VERSION] - $DATE\n\n### Added\n- \n\n### Changed\n- \n\n### Fixed\n- \n\n## [$MAJOR.$MINOR.$PATCH]/" CHANGELOG.md

echo "Version updated to $NEW_VERSION"
echo "Now:"
echo "1. Update the CHANGELOG.md with the actual changes"
echo "2. Run tests: mkdir -p build && cd build && cmake -DENABLE_TESTS=ON .. && make && ctest"
echo "3. Commit changes: git add CMakeLists.txt include/eer.h CHANGELOG.md"
echo "4. Create tag: git tag -a v$NEW_VERSION -m \"Version $NEW_VERSION\""
echo "5. Push changes: git push origin main && git push origin v$NEW_VERSION"
