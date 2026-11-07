# Power plant temperature monitor

Repository: `power-plant-temperature-monitor`  
Language: C++17  
Plan updated: 7 November 2026

Build a command-line application that processes recorded equipment temperatures in a simulated power plant and reports readings that exceed configured alarm thresholds.

The current prototype uses measurements and thresholds defined in C++ source code. The planned application will read both from files, report invalid input, produce per-equipment statistics, and include automated tests. A threshold exceedance currently produces a terminal message; there is no alarm state, acknowledgment, or persistence mechanism.

## Project development plan

The work is divided into stages that leave the application in a runnable state. The status is based on the source reviewed for this plan, not on unverified repository commits or previous lesson attendance.

| Stage | Scope and deliverable | Status |
|---|---|---|
| 1. Represent measurements | Implement `Measurement`, constructor validation, read-only accessors and terminal output | **Implemented and demonstrated** |
| 2. Process multiple readings | Store readings in `std::vector<Measurement>`; iterate, compare against thresholds, print alarms and count exceedances | **Implemented and demonstrated** |
| 3. Introduce threshold configuration and a controller | Add `ThresholdConfig`, equipment-specific overrides and `AlarmController`; move processing behind the controller's public interface | **Implemented in a single-file example; compiled and run** |
| 4. Establish a multi-file build and tests | Extract class declarations and implementations into suitable files; configure CMake; add repeatable automated tests | **Outstanding** |
| 5. Read threshold configuration | Define a small configuration-file format; parse and validate a default threshold and optional equipment overrides | **Outstanding** |
| 6. Read recorded measurements | Define a CSV schema; validate input, report faulty line numbers and apply an explicit invalid-record policy | **Outstanding** |
| 7. Produce equipment summaries | Report count, minimum, maximum, mean and number of exceedances for each equipment ID | **Outstanding** |
| 8. Verify and present the application | Add integration tests, sample input files, a README and a repeatable command-line demonstration | **Outstanding** |

## Progress recorded so far

### Stage 1: Measurement representation

The `Measurement` class holds an equipment identifier (`std::string`), a timestamp (`int`, measured in elapsed seconds) and a temperature (`double`, in degrees Celsius). Its constructor rejects empty identifiers, negative timestamps, and non-finite temperatures. Read-only accessors expose the values without allowing callers to modify the private data members.

The original `measurement_demo.cpp` constructs two measurements and prints their values. Its full lesson is preserved in Part 1 below.

### Stage 2: Multiple measurements and threshold checking

The expanded demonstration stores five readings in a `std::vector<Measurement>`, including multiple readings for the same equipment. A range-based `for` loop processes each measurement. A threshold exceedance is defined as `temperature_celsius > threshold_value`: a reading exactly equal to the threshold does not trigger an alarm.

The example prints each measurement, reports threshold exceedances and computes a total count. A measurement count is not an equipment count: two above-threshold readings from the same equipment contribute two exceedances.

### Stage 3: ThresholdConfig and AlarmController

`ThresholdConfig` stores a default threshold and equipment-specific overrides in an `std::unordered_map<std::string, double>`. It validates the threshold values and configured IDs. `get_threshold()` selects an override using `find()`, or returns the default when none exists.

The latest single-file example, `measurement_demo_central_controller.cpp`, introduces `AlarmController`. The controller owns its `std::vector<Measurement>` and `ThresholdConfig` members. Its public functions, `process_measurements()` and `count_exceedances()`, use private helpers to format readings, compare temperatures and print alarm messages. `main()` constructs the controller and calls those two public functions.

**Verification performed for this update:** The controller example compiled with GCC in C++17 mode using `-Wall -Wextra -Wpedantic -Werror`. Running it produced two alarms, for `generator-01` at 92.5 °C against 90.0 °C and `motor-01` at 88.0 °C against 86.0 °C, followed by `Threshold exceedances: 2`. This is a successful demonstration run, not an automated test suite.

### C++ topics covered alongside implementation

The exercises and discussions have addressed constructor member initializer lists, `const` member functions, references to `const`, pass-by-value and `std::move`, vector element storage and initializer-list construction. The difference between `std::unordered_map::find()` and `contains()` was also covered. `contains()` requires C++20; the C++17 implementation uses `find()` because it needs the associated threshold value.

## Next work: stage 4

**Goal:** Turn the verified single-file example into a maintainable multi-file project without changing its observable behavior.

Suggested initial structure (to be created, not an existing verified directory tree):

```text
power-plant-temperature-monitor/
├── CMakeLists.txt
├── include/
│   ├── alarm_controller.h
│   ├── measurement.h
│   └── threshold_config.h
├── src/
│   ├── alarm_controller.cpp
│   ├── measurement.cpp
│   ├── threshold_config.cpp
│   └── main.cpp
└── tests/
    └── ...
```

Move class declarations into headers and nontrivial function definitions into source files. Keep sample-data construction in the demonstration program for now. Configure CMake for C++17 and enable compiler warnings. Introduce a test executable and register it with CTest; the test framework can be selected when implementing the tests.

Stage 4 is complete when a clean configure-and-build succeeds and automated checks cover:

- Rejection of empty equipment IDs, negative timestamps, NaN and infinite temperatures.
- Rejection of empty override keys and non-finite configured thresholds.
- Selection of default and equipment-specific thresholds.
- No alarm when a temperature equals its threshold.
- The expected two exceedances for the five sample measurements, including repeated equipment IDs.

Tests of terminal output can be added separately if retaining its exact format is a requirement.

## Remaining stages and acceptance criteria

**Stage 5: Configuration file.** Specify a format before implementing a parser. Support one default threshold and zero or more equipment overrides. Define how duplicate IDs, missing fields, malformed numbers and unreadable files are handled. The stage passes when editing the input file changes threshold behavior without recompiling, and invalid configuration produces a useful diagnostic and a nonzero exit status.

**Stage 6: Recorded measurements.** Agree on a CSV header and field conventions, including how equipment IDs are represented and how timestamps are interpreted. Validate field counts, timestamps, temperatures and identifiers before constructing `Measurement` objects. Decide explicitly whether a faulty line stops processing or is reported and skipped. The stage passes when the application processes a valid sample file and reports malformed input with line numbers in accordance with that policy.

**Stage 7: Per-equipment summaries.** Aggregate readings by equipment ID and report measurement count, minimum, maximum, mean temperature and exceedance count. Define whether the summaries include only accepted records and how empty input is displayed. The stage passes when values can be checked by hand against a small sample containing repeated equipment IDs.

**Stage 8: Verification and presentation.** Add end-to-end tests for configuration loading, measurement loading, alarms and summaries. Provide valid and invalid sample files, document build and run commands, and prepare a demonstration from a clean checkout. The stage passes when the documented commands reproduce the expected results.

## Current boundary

The verified demonstration uses source-defined data. It does not establish configuration-file parsing, CSV input, per-equipment statistical summaries, a CMake build or automated test coverage. These remain planned work. No claim is made here about which files have been committed or pushed to GitHub.

---

## Part 1: Represent one measurement

Our first task is to represent a single recorded observation.

A measurement contains:

- An equipment ID.
- A timestamp expressed as elapsed seconds from the start of recording.
- A temperature in degrees Celsius.

For example, `motor-01`, `12` and `85.0` describe a temperature of 85 °C recorded 12 seconds after recording began.

The `Measurement` class will keep these values together. Its constructor will reject:

- An empty equipment ID.
- A negative timestamp.
- A temperature that is infinity or NaN (not a number).

These are the initial project rules. Equipment-specific temperature ranges can be defined when the requirements call for them.

## Step 1: Create the source file

Create a file named `measurement_demo.cpp` in the repository root.

Keep this first program in one file so that we can concentrate on classes, objects and constructors. We will separate the files when we introduce CMake.

Example 1.1: Representing and displaying equipment temperature measurements.

```cpp
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

class Measurement
{
public:
    Measurement(const std::string& equipment_id,
                int timestamp_seconds,
                double temperature_celsius)
        : equipment_id_{equipment_id},
          timestamp_seconds_{timestamp_seconds},
          temperature_celsius_{temperature_celsius}
    {
        if (equipment_id_.empty()) {
            throw std::invalid_argument{
                "Equipment ID must not be empty"
            };
        }

        if (timestamp_seconds_ < 0) {
            throw std::invalid_argument{
                "Timestamp must not be negative"
            };
        }

        if (!std::isfinite(temperature_celsius_)) {
            throw std::invalid_argument{
                "Temperature must be finite"
            };
        }
    }

    const std::string& equipment_id() const
    {
        return equipment_id_;
    }

    int timestamp_seconds() const
    {
        return timestamp_seconds_;
    }

    double temperature_celsius() const
    {
        return temperature_celsius_;
    }

private:
    std::string equipment_id_;
    int timestamp_seconds_;
    double temperature_celsius_;
};

void print_measurement(const Measurement& measurement)
{
    std::cout << measurement.equipment_id()
              << " | t=" << measurement.timestamp_seconds() << " s"
              << " | " << std::fixed << std::setprecision(1)
              << measurement.temperature_celsius() << " deg C\n";
}

int main()
{
    try {
        const Measurement motor_reading{"motor-01", 12, 85.0};
        const Measurement pump_reading{"pump-02", 13, 64.0};

        print_measurement(motor_reading);
        print_measurement(pump_reading);
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
```

This is the complete program for part 1. It creates two measurements and displays them.

## Step 2: Compile and run

Use a C++17-capable compiler. The following commands assume a Linux terminal with GCC installed.

From the directory containing `measurement_demo.cpp`, compile with C++17 and compiler warnings enabled:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic \
    measurement_demo.cpp -o measurement_demo
```

Run the executable:

```bash
./measurement_demo
```

The expected output is:

```text
motor-01 | t=12 s | 85.0 deg C
pump-02 | t=13 s | 64.0 deg C
```

`std::fixed` and `std::setprecision(1)` display the temperature with one digit after the decimal point. The program uses `deg C` in terminal output.

## Step 3: Explain the class and its objects

`Measurement` defines a type. `motor_reading` and `pump_reading` are two objects of that type.

Each object stores its own equipment ID, timestamp and temperature.

Each object represents one observation from a piece of equipment. A later reading from the same motor would be another `Measurement` object.

The data members are private. Code outside the class accesses them through its public member functions.

The member names end with an underscore to distinguish them from constructor parameters.

## Step 4: Explain construction and validation

The constructor receives the three values needed to create a measurement.

Its member initializer list initializes the object's data members before the constructor body checks them. The equipment ID is copied into the object's own `std::string`.

For valid input, construction finishes and the object is ready to use.

For invalid input, the constructor throws `std::invalid_argument`. Construction does not complete. The handler in `main()` prints the error and returns a failure status.

For now, follow the flow:

1. Attempt to construct the object.
2. Check its values.
3. Continue if they are valid.
4. Report the error if construction fails.

A reading of 85 °C can be valid measurement data even when it exceeds the alarm threshold. Construction checks the measurement's validity; a later monitoring operation will check the threshold.

The timestamp uses `int` for this initial example. The file-reading stage will reject values outside the supported range before constructing a measurement.

## Step 5: Explain read-only access

The public access functions return the stored values.

The `const` after each member function means the function cannot modify the object's ordinary data members through `this`.

`equipment_id()` returns a reference to the stored string, avoiding a string copy. That reference remains valid only while the referenced string remains alive. Here, the measurement stays alive throughout printing.

The numeric access functions return their small values by value.

The class provides read-only access because the application processes recorded observations. Both objects in `main()` are also declared `const`.

## Step 6: Explain the separate printing function

`print_measurement()` receives a measurement by reference to `const`. It reads the object without copying it or modifying it.

Printing stays outside the class because representing a measurement and formatting terminal output are separate tasks.

Later, the same measurement can be used for threshold checking, statistics or file output. Each operation can use the public access functions.

## Student exercises

Make one change at a time, then compile and run again.

1. Add a third measurement for `generator-01` at 14 seconds with a temperature of 72.5 °C. Display it.
2. Change one equipment ID to an empty string. Observe the diagnostic.
3. Restore the ID, then use a negative timestamp. Observe the diagnostic.
4. Restore valid values. Explain why two readings from `motor-01` should be separate objects.
5. Explain why a temperature can be valid measurement data and still exceed an alarm threshold.

For the empty-ID exercise, the expected diagnostic is:

```text
Error: Equipment ID must not be empty
```

For the negative-timestamp exercise, the expected diagnostic is:

```text
Error: Timestamp must not be negative
```

The handler ends the program after reporting the error. Measurements are printed only if execution reaches the calls to `print_measurement()`.

## Completion check

Part 1 is complete when you can:

- Compile and run the program.
- Construct and display another measurement.
- Explain the constructor and member initializer list.
- Explain private data and public access functions.
- Follow the error path for invalid constructor arguments.
- Distinguish a recorded measurement from the equipment that produced it.

## Where this lesson leads

Part 1 introduces the `Measurement` class. The subsequent examples extend it to a vector of readings, threshold selection and alarm processing. Those developments are recorded in the progress section above. The Part 1 source code remains unchanged so that the progression from the initial example is clear.
