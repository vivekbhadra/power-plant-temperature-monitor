#include "ThresholdConfig.h"

#include <cmath>
#include <stdexcept>
#include <string>
#include <unordered_map>

ThresholdConfig::ThresholdConfig(double default_threshold_celsius,
                                 std::unordered_map<std::string, double> equipment_thresholds)
    : m_default_threshold(default_threshold_celsius),
      m_equipment_thresholds{std::move(equipment_thresholds)}
{
    if (!std::isfinite(m_default_threshold))
        throw std::invalid_argument("Threshold cannot be an infinite value.");
    for (const auto& [key, value] : equipment_thresholds)
    {
        if (key.empty())
            throw std::invalid_argument("Equipment ID cannot be empty.");
        if (!std::isfinite(value))
            throw std::invalid_argument("Threshold cannot be an infinite value.");
    }
}

double ThresholdConfig::get_threshold(std::string equipment_id) const
{
    auto it = m_equipment_thresholds.find(equipment_id);
    if (it != m_equipment_thresholds.end())
        return it->second;

    return m_default_threshold;
}
