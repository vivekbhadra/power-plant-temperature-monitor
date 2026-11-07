/**
 * @file measurement_demo_central_controller.cpp
 * @brief Demonstrates centralized processing of equipment temperature measurements.
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

class ThresholdConfig
{
public:
    ThresholdConfig(double default_threshold,
                    std::unordered_map<std::string, double> equipment_thresholds)
        : default_threshold_{default_threshold},
          threshold_map_{std::move(equipment_thresholds)}
    {
        if (!std::isfinite(default_threshold_))
        {
            throw std::invalid_argument{"Default threshold must be finite"};
        }

        for (const auto& [equipment_id, threshold_value] : threshold_map_)
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

    double get_threshold(const std::string& equipment_id) const
    {
        const auto position{threshold_map_.find(equipment_id)};
        if (position != threshold_map_.end())
        {
            return position->second;
        }
        return default_threshold_;
    }

private:
    double default_threshold_;
    std::unordered_map<std::string, double> threshold_map_;
};

class Measurement
{
public:
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

/**
 * @brief Owns measurements and their thresholds and handles alarm processing.
 */
class AlarmController
{
public:
    AlarmController(std::vector<Measurement> measurements, ThresholdConfig config)
        : measurements_{std::move(measurements)},
          config_{std::move(config)}
    {
    }

    /** @brief Prints each measurement and raises an alarm if its threshold is exceeded. */
    void process_measurements() const
    {
        for (const auto& measurement : measurements_)
        {
            print_measurement(measurement);

            const double threshold_value{config_.get_threshold(measurement.equipment_id())};
            if (is_above_threshold(measurement, threshold_value))
            {
                raise_alarm(measurement, threshold_value);
            }
        }
    }

    /** @brief Returns the number of measurements strictly above their thresholds. */
    std::size_t count_exceedances() const
    {
        std::size_t count{0};
        for (const auto& measurement : measurements_)
        {
            const double threshold_value{config_.get_threshold(measurement.equipment_id())};
            if (is_above_threshold(measurement, threshold_value))
            {
                ++count;
            }
        }
        return count;
    }

private:
    static void print_measurement(const Measurement& measurement)
    {
        std::cout << measurement.equipment_id() << " | t=" << measurement.timestamp_seconds()
                  << " s | " << std::fixed << std::setprecision(1)
                  << measurement.temperature_celsius() << " deg C\n";
    }

    static bool is_above_threshold(const Measurement& measurement, double threshold_value)
    {
        return measurement.temperature_celsius() > threshold_value;
    }

    static void raise_alarm(const Measurement& measurement, double threshold_value)
    {
        std::cout << "ALARM: " << measurement.equipment_id() << " reached " << std::fixed
                  << std::setprecision(1) << measurement.temperature_celsius()
                  << " deg C; threshold is " << threshold_value << " deg C\n";
    }

    std::vector<Measurement> measurements_;
    ThresholdConfig config_;
};

std::vector<Measurement> create_sample_measurements()
{
    return {{"motor-01", 12, 85.0},
            {"pump-02", 13, 64.0},
            {"generator-01", 14, 92.5},
            {"motor-01", 18, 88.0},
            {"pump-02", 21, 79.5}};
}

ThresholdConfig create_threshold_config()
{
    return {80.0, {{"motor-01", 86.0}, {"generator-01", 90.0}}};
}

int main()
{
    try
    {
        const AlarmController controller{create_sample_measurements(), create_threshold_config()};

        controller.process_measurements();
        std::cout << "Threshold exceedances: " << controller.count_exceedances() << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
