/**
 * @file measurement_demo.cpp
 * @brief Demonstrates validating and printing equipment temperature readings.
 */

#include "AlarmController.h"
#include "Measurement.h"
#include "ThresholdConfig.h"
#include "utility.h"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Processes recorded measurements and reports threshold exceedances.
 * @return 0 if the demonstration succeeds; 1 if an exception is caught.
 */
int main()
{
    try
    {
        AlarmController controller{Utility::createMeasurements(), Utility::createGlobalConfig()};
        controller.monitor();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}

// think of an object with threshold as a parameter
//