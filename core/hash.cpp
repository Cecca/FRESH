/*
 * Copyright 2018 Matteo Ceccarello
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "hash.h"
#include "io.h"
#include "timer.h"

std::vector<size_t> g_tl_tensored_multihash_scratch_left;
std::vector<size_t> g_tl_tensored_multihash_scratch_right;
curve_to_idx_t g_tl_scratch_curve_to_idx;
std::vector<size_t> g_tl_hash_values;


