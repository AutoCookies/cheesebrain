#include "cheese-power.h"
#include "cheese-impl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#ifdef __linux__
#include <unistd.h>
#endif

namespace {
    std::vector<cheese_power_state_observer *> observers;
}

cheese_power_state cheese_power_query_state() {
    cheese_power_state state = {
        CHEESE_THERMAL_STATE_NOMINAL,
        CHEESE_POWER_PROFILE_BALANCED,
        1.0f,
        true
    };

#ifdef __linux__
    // Basic thermal state query on Linux via /sys/class/thermal
    std::ifstream thermal_file("/sys/class/thermal/thermal_zone0/temp");
    if (thermal_file.is_open()) {
        int temp;
        thermal_file >> temp;
        temp /= 1000; // to Celsius
        if (temp > 80) state.thermal = CHEESE_THERMAL_STATE_CRITICAL;
        else if (temp > 65) state.thermal = CHEESE_THERMAL_STATE_HIGH;
        else if (temp > 50) state.thermal = CHEESE_THERMAL_STATE_FAIR;
    }

    // Basic battery level query on Linux via /sys/class/power_supply
    std::ifstream capacity_file("/sys/class/power_supply/BAT0/capacity");
    if (capacity_file.is_open()) {
        int capacity;
        capacity_file >> capacity;
        state.battery_level = capacity / 100.0f;
    }

    std::ifstream status_file("/sys/class/power_supply/BAT0/status");
    if (status_file.is_open()) {
        std::string status;
        status_file >> status;
        state.is_charging = (status == "Charging");
    }
#endif

    return state;
}

void cheese_power_monitor_init() {
    // In a real implementation, this would start a background thread
    // that polls cheese_power_query_state() and notifies observers.
}

void cheese_power_monitor_add_observer(cheese_power_state_observer * observer) {
    observers.push_back(observer);
}

void cheese_power_monitor_remove_observer(cheese_power_state_observer * observer) {
    auto it = std::find(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it);
    }
}
