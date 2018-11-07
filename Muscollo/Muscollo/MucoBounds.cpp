/* -------------------------------------------------------------------------- *
 * OpenSim Muscollo: MucoBounds.cpp                                           *
 * -------------------------------------------------------------------------- *
 * Copyright (c) 2017 Stanford University and the Authors                     *
 *                                                                            *
 * Author(s): Nicholas Bianco                                                 *
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

#include "MucoBounds.h"

using namespace OpenSim;

MucoBounds::MucoBounds() {
    constructProperties();
}

MucoBounds::MucoBounds(double value) : MucoBounds() {
    append_bounds(value);
}

MucoBounds::MucoBounds(double lower, double upper) : MucoBounds() {
    OPENSIM_THROW_IF(lower > upper, Exception,
        "Expected lower <= upper, but lower=" + std::to_string(lower)
        + " and upper=" + std::to_string(upper) + ".");
    append_bounds(lower);
    append_bounds(upper);
}

void MucoBounds::constructProperties() {
    constructProperty_bounds();
}

void MucoBounds::printDescription(std::ostream& stream) const {
    if (isEquality()) {
        stream << getLower();
    }
    else {
        stream << "[" << getLower() << ", " << getUpper() << "]";
    }
    stream.flush();
}