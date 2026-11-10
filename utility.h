#pragma once

#include "Measurement.h"
#include "ThresholdConfig.h"

#include <vector>

namespace Utility
{
void print_measurement(const Measurement& measurement);
bool exceeds_threshold(const Measurement& measurement, double threshold_celsius);
std::size_t count_exceedances(const std::vector<Measurement>& measurements,
                              double threshold_celsius);
std::vector<Measurement> createMeasurements();
ThresholdConfig createGlobalConfig();
} // namespace Utility