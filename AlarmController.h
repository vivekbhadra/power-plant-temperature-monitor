#pragma once

#include "Measurement.h"
#include "ThresholdConfig.h"

#include <iostream>
#include <vector>

class AlarmController
{
public:
    AlarmController(std::vector<Measurement> measurements, ThresholdConfig config);

    void monitor();

private:
    void processMeasurements();

    void report_alarm(const Measurement& measurement, double threshold_celsius);

    std::vector<Measurement> m_measurements;
    ThresholdConfig m_config;
};