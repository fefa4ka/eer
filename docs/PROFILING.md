## Profiling

The EER Framework includes built-in profiling capabilities to help identify performance bottlenecks and optimize your components.

### Enabling Profiling

To enable profiling, build your project with the `PROFILING` option:

```bash
mkdir -p build && cd build
cmake -DPROFILING=ON -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Profiling Output

When profiling is enabled:

1. Each component's lifecycle methods are timed
2. CPU usage is tracked for each method
3. Call counts are recorded
4. Hardware abstraction layer (HAL) calls are monitored

The profiling data is automatically dumped when:
- The program exits normally
- You press Ctrl+C (SIGINT is caught)
- The `eer_dump_usage()` function is called explicitly

### Profiling Report

The profiling report includes:

```
CPU usage details:

operation               steps   cpu     %

3 components            1000    12345   100%

0: MyComponent / Button
 will_mount             5       123     0.99%
   some_call_counter    10
 should_update          100     456     3.69%
 will_update            95      789     6.39%
 release                95      1011    8.19%
 did_update             95      234     1.90%
                                2613    21.17%


1: AnotherComponent / Slider
...
```

This shows:
- Total number of components and event loop steps
- Per-component CPU usage breakdown
- Per-lifecycle-method timing and percentage of total CPU time
- Hardware call counts within each method

### Analyzing Profiling Data

Use the profiling data to:

1. **Identify Bottlenecks**: Look for methods consuming a high percentage of CPU time
2. **Optimize Update Logic**: Check if `should_update` is properly filtering unnecessary updates
3. **Reduce Hardware Calls**: Look for excessive hardware calls in frequently executed methods
4. **Balance Component Load**: Ensure no single component dominates CPU usage

### Custom Profiling Points

You can add custom profiling points in your code:

```c
#ifdef PROFILING
  // Custom profiling code
  eer_profiler_counter(some_custom_counter);
#endif
```

### Memory Profiling

The profiling system also tracks component allocation. Check the report for:
- Number of components created
- Component lifecycle patterns
- Potential memory leaks (components that never unmount)
