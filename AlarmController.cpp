#include "AlarmController.h"

#include "Measurement.h"
#include "ThresholdConfig.h"
#include "utility.h"

#include <iomanip>
#include <iostream>
#include <vector>

AlarmController::AlarmController(std::vector<Measurement> measurements, ThresholdConfig config)
    : m_measurements(measurements),
      m_config(config)
{
}

void AlarmController::monitor()
{
    processMeasurements();
}

void AlarmController::processMeasurements()
{
    for (const auto& measurement : m_measurements)
    {
        Utility::print_measurement(measurement);

        if (Utility::exceeds_threshold(measurement,
                                       m_config.get_threshold(measurement.equipment_id())))
        {
            report_alarm(measurement, m_config.get_threshold(measurement.equipment_id()));
        }
    }
}

void AlarmController::report_alarm(const Measurement& measurement, double threshold_celsius)
{
    std::cout << "ALARM: " << measurement.equipment_id() << " reached " << std::fixed
              << std::setprecision(1) << measurement.temperature_celsius()
              << " deg C; threshold is " << threshold_celsius << " deg C\n";
}

std::vector<Measurement> m_measurements;
ThresholdConfig m_config;
