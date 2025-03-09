# Changelog

All notable changes to the EER Framework will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2025-03-09

### Added
- Comprehensive component lifecycle management
- Null pointer checks for lifecycle methods
- Debug utilities for component inspection
- Improved documentation with detailed API reference
- Memory optimization for component structures

### Changed
- Reorganized header files for better modularity
- Improved error handling in component lifecycle
- Enhanced component unmounting process

### Fixed
- Fixed null pointer issue in `did_unmount` lifecycle method
- Fixed component state transitions during unmounting

## [0.1.0] - Initial Release

### Added
- Basic component lifecycle (mount, update, unmount)
- Event loop implementation with `loop` and `ignite`/`terminate`/`halt` approaches
- Component props and state management
- Minimal memory footprint for embedded systems
