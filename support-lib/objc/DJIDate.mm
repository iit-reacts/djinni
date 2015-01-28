//
// Copyright 2014 Dropbox, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "DJIDate.h"

namespace djinni {

std::chrono::system_clock::time_point convert_date(const double seconds_since_epoch) {
    static const auto POSIX_EPOCH = std::chrono::system_clock::from_time_t(0);
    const std::chrono::duration<double> converted_date(seconds_since_epoch);
    const auto converted_system_date = std::chrono::duration_cast<std::chrono::system_clock::duration>(converted_date);
    return POSIX_EPOCH + converted_system_date;
}

} // namespace djinni
