#ifndef MOCO_SMOOTHBHARGAVA2004METABOLICS_H
#define MOCO_SMOOTHBHARGAVA2004METABOLICS_H
/* -------------------------------------------------------------------------- *
 * OpenSim Moco: SmoothBhargava2004Metabolics.                              *
 * -------------------------------------------------------------------------- *
 * Copyright (c) 2019 Stanford University and the Authors                     *
 *                                                                            *
 * Author(s): Christopher Dembia                                              *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0          *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#include "../osimMocoDLL.h"
#include <unordered_map>

#include <OpenSim/Simulation/Model/ModelComponent.h>
#include <OpenSim/Simulation/Model/Muscle.h>

namespace OpenSim {

/// Object class that holds the metabolic parameters required to calculate
/// metabolic power for a single muscle.
class OSIMMOCO_API SmoothBhargava2004Metabolics_MetabolicMusclePrameters :
        public Object {
    OpenSim_DECLARE_CONCRETE_OBJECT(
            SmoothBhargava2004Metabolics_MetabolicMusclePrameters, Object);
public:
    OpenSim_DECLARE_PROPERTY(specific_tension, double,
        "The specific tension of the muscle (Pascals (N/m^2)).");
    OpenSim_DECLARE_PROPERTY(density, double,
        "The density of the muscle (kg/m^3).");
    OpenSim_DECLARE_PROPERTY(ratio_slow_twitch_fibers, double,
        "Ratio of slow twitch fibers in the muscle "
        "(must be between 0 and 1).");
    OpenSim_DECLARE_OPTIONAL_PROPERTY(use_provided_muscle_mass, bool,
        "An optional flag that allows the user to explicitly specify a muscle "
        "mass. If set to true, the 'provided_muscle_mass' property must be "
        "specified.");
    OpenSim_DECLARE_OPTIONAL_PROPERTY(provided_muscle_mass, double,
        "The user specified muscle mass (kg).");
    OpenSim_DECLARE_PROPERTY(activation_constant_slow_twitch, double,
        "Activation constant for slow twitch fibers (W/kg).");
    OpenSim_DECLARE_PROPERTY(activation_constant_fast_twitch, double,
        "Activation constant for fast twitch fibers (W/kg).");
    OpenSim_DECLARE_PROPERTY(maintenance_constant_slow_twitch, double,
        "Maintenance constant for slow twitch fibers (W/kg).");
    OpenSim_DECLARE_PROPERTY(maintenance_constant_fast_twitch, double,
        "Maintenance constant for fast twitch fibers (W/kg).");
    SmoothBhargava2004Metabolics_MetabolicMusclePrameters();
    SmoothBhargava2004Metabolics_MetabolicMusclePrameters(
            const std::string& muscleName,
            double ratio_slow_twitch_fibers,
            double muscle_mass = SimTK::NaN);
    SmoothBhargava2004Metabolics_MetabolicMusclePrameters(
        const std::string& muscleName,
        double ratio_slow_twitch_fibers,
        double activation_constant_slow_twitch,
        double activation_constant_fast_twitch,
        double maintenance_constant_slow_twitch,
        double maintenance_constant_fast_twitch,
        double muscle_mass = SimTK::NaN);

    const double getMuscleMass() const { return m_muscleMass; }
    void setMuscleMass();

private:
    void constructProperties();
    mutable SimTK::ReferencePtr<const Muscle> m_muscle;
    mutable double m_muscleMass;
};

/// Object class that holds the set of metabolic parameters required to
/// calculate metabolic power for a single muscle.
class OSIMMOCO_API
    SmoothBhargava2004Metabolics_MetabolicMusclePrameterSet
    : public Set<SmoothBhargava2004Metabolics_MetabolicMusclePrameters>
{
    OpenSim_DECLARE_CONCRETE_OBJECT(
        SmoothBhargava2004Metabolics_MetabolicMusclePrameterSet,
        Set<SmoothBhargava2004Metabolics_MetabolicMusclePrameters>);

public:
    SmoothBhargava2004Metabolics_MetabolicMusclePrameterSet() {}
};


/// Metabolic energy model from Bhargava et al (2004).
/// https://doi.org/10.1016/s0021-9290(03)00239-2
class OSIMMOCO_API SmoothBhargava2004Metabolics : public ModelComponent {
    OpenSim_DECLARE_CONCRETE_OBJECT(
            SmoothBhargava2004Metabolics, ModelComponent);

public:
    OpenSim_DECLARE_OUTPUT(total_metabolic_rate, double, getTotalMetabolicRate,
            SimTK::Stage::Velocity);
    OpenSim_DECLARE_LIST_OUTPUT(muscle_metabolic_rate, double,
            getMuscleMetabolicRate, SimTK::Stage::Velocity);

    double getTotalMetabolicRate(const SimTK::State& s) const;
    double getMuscleMetabolicRate(
            const SimTK::State& s, const std::string& channel) const;

private:
    void extendConnectToModel(Model&) override;
    void extendAddToSystem(SimTK::MultibodySystem&) const override;
    const SimTK::Vector& getMetabolicRate(const SimTK::State& s) const;
    void calcMetabolicRate(const SimTK::State& s, SimTK::Vector&) const;

    mutable std::vector<std::pair<SimTK::ReferencePtr<const Muscle>, double>>
            m_muscles;
    mutable std::unordered_map<std::string, int> m_muscleIndices;
};

} // namespace OpenSim

#endif // MOCO_SMOOTHBHARGAVA2004METABOLICS_H
