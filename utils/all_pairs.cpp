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
#include "frechet.h"
#include "io.h"
#include "threading.h"
#include "types.h"
#include <random>
#include "rand.h"

extern std::vector<double> tmp1;
extern std::vector<double> tmp2;
#pragma omp threadprivate(tmp1, tmp2)
std::vector<double> tmp1;
std::vector<double> tmp2;

template<typename Point>
int main_impl(std::string & path) {
  std::vector<Curve<Point>> dataset = load_dataset_msgpack<Point>(path);

  uint64_t seed;
  std::random_device rd;
  seed = rd();
  Splitmix64 seeder(seed);
  Xorshift1024star rnd(seeder.next());

  auto ts = new_thread_states(rnd);

  size_t n = dataset.size();
  size_t n_pairs = n*(n-1)/2;
  size_t max_pairs = 499500; // 1000 \choose 2

  double prob = std::min(1.0, max_pairs / (double) n_pairs);

  std::cerr << "Loaded dataset of " << n << " curves (" << n_pairs 
            << " pairs) sampling with probability " << prob << std::endl;
#pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
    auto tid = omp_get_thread_num();
    const std::vector<Point> &a = dataset[i].points;
    for (size_t j = i + 1; j < n; j++) {
      if (ts[tid].rnd.next_double() <= prob) {
        const std::vector<Point> &b = dataset[j].points;
        double d = discrete_frechet_distance(a, b, tmp1, tmp2);
#pragma omp critical
        {
          std::cout << dataset[i].id << " " << dataset[j].id << " " << d
                    << std::endl;
        }
      }
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "USAGE: " << argv[0] << " DATASET" << std::endl;
    return 1;
  }
  std::string path(argv[1]);

  switch(read_dataset_dimensions(path)) {
    case 1:
      return main_impl<Point1D>(path);
    case 2:
      return main_impl<Point2D>(path);
    default:
      std::cerr << "Unsupported number of dimensions" << std::endl;
      return -1;
  }
}
