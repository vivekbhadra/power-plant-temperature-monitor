#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
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
    Measurement(const std::string& equipment_id, int timestamp_seconds, double temperature_celsius);

    /**
     * @brief Returns the identifier of the measured equipment.
     * @return A constant reference to this measurement's equipment identifier.
     *
     * The returned reference remains valid only while this Measurement object
     * remains alive and is not otherwise modified or destroyed.
     */
    const std::string& equipment_id() const;

    /**
     * @brief Returns the time at which this measurement was taken.
     * @return The timestamp in seconds.
     */
    int timestamp_seconds() const;

    /**
     * @brief Returns the measured temperature.
     * @return The temperature in degrees Celsius.
     */
    double temperature_celsius() const;

private:
    std::string equipment_id_;
    int timestamp_seconds_;
    double temperature_celsius_;
};
