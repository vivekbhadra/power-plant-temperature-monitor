#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

class ThresholdConfig
{
public:
    ThresholdConfig() = default;
    ThresholdConfig(double default_threshold_celsius,
                    std::unordered_map<std::string, double> equipment_thresholds);

    double get_threshold(std::string equipment_id) const;

private:
    double m_default_threshold;
    std::unordered_map<std::string, double> m_equipment_thresholds;
};