#pragma once

#include <cstdint>
#include <vector>

enum cheese_thermal_state {
    CHEESE_THERMAL_STATE_NOMINAL = 0,
    CHEESE_THERMAL_STATE_FAIR    = 1,
    CHEESE_THERMAL_STATE_HIGH    = 2,
    CHEESE_THERMAL_STATE_CRITICAL = 3,
};

enum cheese_power_profile {
    CHEESE_POWER_PROFILE_BALANCED = 0,
    CHEESE_POWER_PROFILE_PERFORMANCE = 1,
    CHEESE_POWER_PROFILE_POWER_SAVE = 2,
};

struct cheese_power_state {
    cheese_thermal_state thermal;
    cheese_power_profile profile;
    float battery_level; // 0.0 to 1.0
    bool is_charging;
};

class cheese_power_state_observer {
public:
    virtual ~cheese_power_state_observer() = default;
    virtual void on_power_state_changed(const cheese_power_state & state) = 0;
};

// Platform-specific power state query
cheese_power_state cheese_power_query_state();

// Global power monitor
void cheese_power_monitor_init();
void cheese_power_monitor_add_observer(cheese_power_state_observer * observer);
void cheese_power_monitor_remove_observer(cheese_power_state_observer * observer);
