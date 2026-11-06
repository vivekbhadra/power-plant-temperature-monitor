/**
 * @file measurement_demo.cpp
 * @brief Demonstrates validating and printing equipment temperature readings.
 */

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * @brief Stores the default alarm threshold and equipment-specific overrides.
 */
class MonitorConfig
{
public:
    /**
     * @brief Constructs and validates the monitoring configuration.
     * @param default_threshold_celsius Threshold used when no equipment-specific
     *        threshold exists.
     * @param equipment_thresholds Equipment IDs and their threshold overrides.
     * @throws std::invalid_argument If a threshold is not finite or a configured
     *         equipment ID is empty.
     */
    MonitorConfig(double default_threshold_celsius,
                  std::unordered_map<std::string, double> equipment_thresholds)
        : default_threshold_celsius_{default_threshold_celsius},
          equipment_thresholds_{std::move(equipment_thresholds)}
    {
        if (!std::isfinite(default_threshold_celsius_))
        {
            throw std::invalid_argument{"Default threshold must be finite"};
        }

        for (const auto& [equipment_id, threshold_value] : equipment_thresholds_)
        {
            if (equipment_id.empty())
            {
                throw std::invalid_argument{"Configured equipment ID must not be empty"};
            }

            if (!std::isfinite(threshold_value))
            {
                throw std::invalid_argument{"Equipment threshold must be finite"};
            }
        }
    }

    /**
     * @brief Returns the threshold configured for an equipment ID.
     * @param equipment_id Equipment whose threshold is required.
     * @return The equipment-specific threshold when present; otherwise, the
     *         default threshold.
     */
    double get_equipment_threshold(const std::string& equipment_id) const
    {
        const auto position{equipment_thresholds_.find(equipment_id)};

        if (position != equipment_thresholds_.end())
        {
            return position->second;
        }

        return default_threshold_celsius_;
    }

private:
    double default_threshold_celsius_;
    std::unordered_map<std::string, double> equipment_thresholds_;
};

/**
 * @brief A validated temperature reading associated with equipment.
 *
 * Timestamps are represented in seconds, and temperatures in degrees Celsius.
 */
class Measurement
{
public:
    /**
     * @brief Constructs a measurement after validating its values.
     * @param equipment_id Non-empty identifier of the measured equipment.
     * @param timestamp_seconds Measurement timestamp in seconds; must be non-negative.
     * @param temperature_celsius Measured temperature in degrees Celsius; must be finite.
     * @throws std::invalid_argument If the equipment ID is empty, the timestamp
     *         is negative, or the temperature is not finite.
     */
    Measurement(const std::string& equipment_id, int timestamp_seconds, double temperature_celsius)
        : equipment_id_{equipment_id},
          timestamp_seconds_{timestamp_seconds},
          temperature_celsius_{temperature_celsius}
    {
        if (equipment_id_.empty())
        {
            throw std::invalid_argument{"Equipment ID must not be empty"};
        }

        if (timestamp_seconds_ < 0)
        {
            throw std::invalid_argument{"Timestamp must not be negative"};
        }

        if (!std::isfinite(temperature_celsius_))
        {
            throw std::invalid_argument{"Temperature must be finite"};
        }
    }

    /**
     * @brief Returns the identifier of the measured equipment.
     * @return A constant reference to this measurement's equipment identifier.
     *
     * The returned reference remains valid only while this Measurement object
     * remains alive and is not otherwise modified or destroyed.
     */
    const std::string& equipment_id() const
    {
        return equipment_id_;
    }

    /**
     * @brief Returns the time at which this measurement was taken.
     * @return The timestamp in seconds.
     */
    int timestamp_seconds() const
    {
        return timestamp_seconds_;
    }

    /**
     * @brief Returns the measured temperature.
     * @return The temperature in degrees Celsius.
     */
    double temperature_celsius() const
    {
        return temperature_celsius_;
    }

private:
    std::string equipment_id_;
    int timestamp_seconds_;
    double temperature_celsius_;
};

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
 * @brief Determines whether a measurement is above the alarm threshold.
 * @param measurement The measurement to evaluate.
 * @param threshold_value Alarm threshold in degrees Celsius.
 * @return true if the measured temperature is strictly above the threshold;
 *         otherwise, false.
 */
bool is_above_threshold(const Measurement& measurement, double threshold_value)
{
    return measurement.temperature_celsius() > threshold_value;
}

/**
 * @brief Prints an alarm for a threshold-exceeding measurement.
 * @param measurement The measurement that exceeded the threshold.
 * @param threshold_value Alarm threshold in degrees Celsius.
 */
void raise_alarm(const Measurement& measurement, double threshold_value)
{
    std::cout << "ALARM: " << measurement.equipment_id() << " reached " << std::fixed
              << std::setprecision(1) << measurement.temperature_celsius()
              << " deg C; threshold is " << threshold_value << " deg C\n";
}

/**
 * @brief Counts measurements that exceed the alarm threshold.
 * @param measurements Measurements to examine.
 * @param config Configuration used to select the threshold for each measurement.
 * @return The number of measurements strictly above their selected thresholds.
 */
std::size_t count_exceedances(const std::vector<Measurement>& measurements,
                              const MonitorConfig& config)
{
    std::size_t count{0};

    for (const auto& measurement : measurements)
    {
        const double threshold_value{config.get_equipment_threshold(measurement.equipment_id())};

        if (is_above_threshold(measurement, threshold_value))
        {
            ++count;
        }
    }

    return count;
}

/**
 * @brief Processes recorded measurements and reports threshold exceedances.
 * @return 0 if the demonstration succeeds; 1 if an exception is caught.
 */
int main()
{
    try
    {
        const std::vector<Measurement> measurements{{"motor-01", 12, 85.0},
                                                    {"pump-02", 13, 64.0},
                                                    {"generator-01", 14, 92.5},
                                                    {"motor-01", 18, 88.0},
                                                    {"pump-02", 21, 79.5}};

        const MonitorConfig config{80.0, {{"motor-01", 86.0}, {"generator-01", 90.0}}};

        for (const auto& measurement : measurements)
        {
            print_measurement(measurement);

            const double threshold_value{config.get_equipment_threshold(measurement.equipment_id())};

            if (is_above_threshold(measurement, threshold_value))
            {
                raise_alarm(measurement, threshold_value);
            }
        }

        std::cout << "Threshold exceedances: " << count_exceedances(measurements, config) << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
