# EER Framework Release Process

## Version Numbering

The EER Framework follows [Semantic Versioning](https://semver.org/):

- **MAJOR** version for incompatible API changes
- **MINOR** version for backwards-compatible functionality additions
- **PATCH** version for backwards-compatible bug fixes

## Version Information

Version information is stored in three places:

1. **CMakeLists.txt**: Contains the authoritative version numbers
   ```cmake
   set(EER_VERSION_MAJOR 0)
   set(EER_VERSION_MINOR 2)
   set(EER_VERSION_PATCH 0)
   set(EER_VERSION "${EER_VERSION_MAJOR}.${EER_VERSION_MINOR}.${EER_VERSION_PATCH}")
   ```

2. **include/eer.h**: Contains version macros for runtime checking
   ```c
   #define EER_VERSION_MAJOR 0
   #define EER_VERSION_MINOR 2
   #define EER_VERSION_PATCH 0
   #define EER_VERSION_STRING "0.2.0"
   ```

3. **Git tags**: Each release is tagged in the repository

## Release Checklist

### Pre-release

1. **Update Version Numbers**
   - Update version numbers in `CMakeLists.txt`
   - Update version macros in `include/eer.h`
   - Ensure both are in sync

2. **Update Documentation**
   - Update API documentation if needed
   - Update `CHANGELOG.md` with all changes since the last release
   - Ensure README examples are up to date

3. **Run Tests**
   ```bash
   mkdir -p build && cd build
   cmake -DENABLE_TESTS=ON ..
   make
   ctest
   ```

4. **Build Examples**
   ```bash
   mkdir -p build && cd build
   cmake -DBUILD_EXAMPLES=ON ..
   make
   ```

### Release

1. **Commit Version Changes**
   ```bash
   git add CMakeLists.txt include/eer.h CHANGELOG.md
   git commit -m "Bump version to X.Y.Z"
   ```

2. **Create Release Tag**
   ```bash
   git tag -a vX.Y.Z -m "Version X.Y.Z"
   ```

3. **Push Changes**
   ```bash
   git push origin main
   git push origin vX.Y.Z
   ```

4. **Create GitHub Release**
   - Go to GitHub Releases page
   - Create a new release from the tag
   - Include release notes from CHANGELOG.md
   - Attach binary artifacts if applicable

### Post-release

1. **Announce Release**
   - Announce on relevant channels
   - Update documentation website if applicable

2. **Prepare for Next Development Cycle**
   - Create a new section in CHANGELOG.md for the next version
   - Consider bumping version to next development version (X.Y.Z-dev)

## Hotfix Process

For critical bug fixes that need to be released outside the normal release cycle:

1. Create a branch from the release tag
   ```bash
   git checkout -b hotfix/X.Y.Z vX.Y.(Z-1)
   ```

2. Fix the bug and update version numbers
   - Increment only the PATCH version

3. Follow the normal release process for the hotfix

## Compatibility Policy

- **MAJOR** version changes may break API compatibility
- **MINOR** and **PATCH** version changes must maintain backward compatibility
- Deprecation notices should be added at least one MINOR version before removal

## Version Checking in Code

Applications can check the EER Framework version at compile time:

```c
#if EER_VERSION_AT_LEAST(0, 2, 0)
    // Use features available in 0.2.0 or later
#else
    // Use older approach
#endif
```

Or at runtime:

```c
printf("EER Framework version: %s\n", EER_VERSION_STRING);
```
