#include "Measurement.h"

#include <cmath>
#include <stdexcept>
#include <string>

Measurement::Measurement(const std::string& equipment_id,
                         int timestamp_seconds,
                         double temperature_celsius)
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

const std::string& Measurement::equipment_id() const
{
    return equipment_id_;
}

int Measurement::timestamp_seconds() const
{
    return timestamp_seconds_;
}

double Measurement::temperature_celsius() const
{
    return temperature_celsius_;
}
