
#include "utility.h"

#include "Measurement.h"
#include "ThresholdConfig.h"

#include <iomanip>
#include <iostream>
#include <vector>

namespace Utility
{
/**
 * @brief Prints a measurement to standard output.
 * @param measurement The measurement to print.
 *
 * The timestamp is printed in seconds and the temperature is printed in
 * degrees Celsius with one digit after the decimal point.
 */
void print_measurement(const Measurement& measurement)
{
    std::cout << measurement.equipment_id() << " | t=" << measurement.timestamp_seconds() << " s"
              << " | " << std::fixed << std::setprecision(1) << measurement.temperature_celsius()
              << " deg C\n";
}

/**
 * @brief Determines whether a measurement exceeds the alarm threshold.
 * @param measurement The measurement to evaluate.
 * @param threshold_celsius Alarm threshold in degrees Celsius.
 * @return true if the measured temperature is strictly above the threshold;
 *         otherwise, false.
 */
bool exceeds_threshold(const Measurement& measurement, double threshold_celsius)
{
    return measurement.temperature_celsius() > threshold_celsius;
}

/**
 * @brief Counts measurements that exceed the alarm threshold.
 * @param measurements Measurements to examine.
 * @param threshold_celsius Alarm threshold in degrees Celsius.
 * @return The number of measurements strictly above the threshold.
 */
std::size_t count_exceedances(const std::vector<Measurement>& measurements,
                              double threshold_celsius)
{
    std::size_t count{0};

    for (const auto& measurement : measurements)
    {
        if (exceeds_threshold(measurement, threshold_celsius))
        {
            ++count;
        }
    }

    return count;
}

std::vector<Measurement> createMeasurements()
{
    return {{"motor-01", 12, 85.0},
            {"pump-02", 13, 64.0},
            {"generator-01", 14, 92.5},
            {"motor-01", 18, 88.0},
            {"pump-02", 21, 79.5}};
}
ThresholdConfig createGlobalConfig()
{
    return ThresholdConfig{80.0, {{"pump-02", 70.0}, {"generator-01", 75.0}}};
}
}; // namespace Utility