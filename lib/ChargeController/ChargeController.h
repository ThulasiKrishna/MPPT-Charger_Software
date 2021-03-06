/* mbed library for a battery charge controller
 * Copyright (c) 2017 Martin Jäger (www.libre.solar)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// settings: battery_capacity, charge_profile
// input:  actual charge current, actual battery voltage
// output: SOC, target_voltage, target_current (=max current)

#ifndef CHARGECONTROLLER_H
#define CHARGECONTROLLER_H

#include "mbed.h"

struct ChargingProfile {
    int num_cells;

    // State: Standby
    int time_limit_recharge; // sec
    float cell_voltage_recharge; // V

    // State: CC/bulk
    float charge_current_max;  // A        PCB maximum: 20 A

    // State: CV/absorption
    float cell_voltage_max;        // max voltage per cell
    int time_limit_CV; // sec
    float current_cutoff_CV; // A

    // State: float/trickle
    bool trickle_enabled;
    float cell_voltage_trickle;    // target voltage for trickle charging of lead-acid batteries
    int time_trickle_recharge;     // sec

    // State: equalization
    bool equalization_enabled;
    float cell_voltage_equalization; // V
    int time_limit_equalization; // sec
    float current_limit_equalization; // A
    int equalization_trigger_time; // weeks
    int equalization_trigger_deep_cycles; // times

    float cell_voltage_load_disconnect;
    float cell_voltage_load_reconnect;

    // TODO
    float temperature_compensation;
};

// possible charger states
enum charger_states {CHG_IDLE, CHG_CC, CHG_CV, CHG_TRICKLE, CHG_EQUALIZATION};

class ChargeController
{
public:

    /** Create Charge Controller object
     *
     *  @param profile ChargingProfile struct including voltage limits, etc.
     */
    ChargeController(ChargingProfile& profile);

    /** Get target battery current for current charger state
     *
     *  @returns
     *    Target current (A)
     */
    float read_target_current();

    /** Get target battery voltage for current charger state
     *
     *  @returns
     *    Target voltage (V)
     */
    float read_target_voltage();

    /** Determine if charging of the battery is allowed
     *
     *  @returns
     *    True if charging is allowed
     */
    bool charging_enabled();

    /** Determine if discharging the battery is allowed
     *
     *  @returns
     *    True if discharging is allowed
     */
    bool discharging_enabled();

    /** Charger state machine update, should be called exactly once per second
     *
     *  @param battery_voltage Actual measured battery voltage (V)
     *  @param battery_current Actual measured battery current (A)
     */
    void update(float battery_voltage, float battery_current);

    /** Get current charge controller state
     *
     *  @returns
     *    CHG_IDLE, CHG_CC, CHG_CV, CHG_TRICKLE or CHG_EQUALIZATION
     */
    int get_state();

private:
    /** Enter a different charger state
     *
     *  @param next_state Next state (e.g. CHG_CC)
     */
    void enter_state(int next_state);

    ChargingProfile& _profile;          // all charging profile variables
    int _state;                         // valid states: enum charger_states
    int _time_state_changed;            // timestamp of last state change
    float _target_voltage;              // target voltage for current state
    float _target_current;              // target current for current state
    int _time_voltage_limit_reached;    // last time the CV limit was reached
    bool _charging_enabled;
    bool _discharging_enabled;
};


#endif // CHARGECONTROLLER_H
