# Power plant temperature monitor

Repository name: `power-plant-temperature-monitor`

Build a command-line C++ prototype for monitoring equipment temperatures in a simulated power plant environment and raising an alarm when a reading exceeds a user-defined threshold.

The completed first version will read a threshold from a configuration file at startup, process recorded measurements, report invalid records, display threshold exceedances and produce a summary for each equipment ID. It will run on a PC using sample data.

## Project development plan

We will build the application in small, working stages. Each stage extends the previous one.

The first version uses one configured temperature threshold for all equipment. An alarm is a terminal message for each reading strictly above that threshold.

| Stage | Work | Working result |
|---|---|---|
| 1. Represent a measurement | Create a `Measurement` class, construct objects and display their values | A compilable program using two measurements |
| 2. Process several measurements | Introduce `std::vector`, loops and a threshold comparison function | Display an alarm for each reading above a temporary threshold |
| 3. Introduce the project build | Separate headers and source files; add CMake and automated tests | Build and test a small multi-file application |
| 4. Load configuration | Read and validate the temperature threshold from a configuration file | Change the threshold without recompiling |
| 5. Load recorded measurements | Parse the agreed CSV format; report line numbers and invalid records | Process measurements from a file |
| 6. Produce equipment summaries | Group measurements and calculate count, minimum, maximum, mean and exceedance count | Display a summary for each equipment ID |
| 7. Complete and present | Add integration tests, sample files, documentation and a demonstration | A GitHub project the student can explain and demonstrate |

The temporary threshold in stage 2 lets us develop the comparison separately. Stage 4 replaces it with the configuration-file value. Each stage can be taught over one or more lessons, depending on the student's progress.

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

## Next stage

Part 2 will store several measurements in a `std::vector`, examine them with a loop and display alarms for temperatures above a temporary threshold. The configuration-file reader will supply that threshold in stage 4.
