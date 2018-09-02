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
#include "prelude.h"
#include <random>
#include <boost/filesystem.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

namespace fs = boost::filesystem;

std::string dataset_name() {
  char * env = std::getenv("TEST_DATASET");
  if (env == nullptr) {
    throw std::runtime_error("You should set the TEST_DATASET variable");
  }
  return std::string(env);
}


/* std::vector<std::string> read_paths() { */
/*   fs::path d("test_curves"); */
/*   if (!fs::exists(d)) { */
/*     FAIL("Directory not found"); */
/*   } */
/*   fs::directory_iterator dend; */
/*   std::vector<std::string> paths; */
/*   for (auto dit = fs::directory_iterator(d); dit != dend; dit++) { */
/*     paths.push_back(dit->path().native()); */
/*   } */
/*   return paths; */
/* } */

/* std::vector<Curve<Point2D>> load_dataset(dataset_name()) { */
/*   auto paths = read_paths(); */
/*   std::vector<Curve<Point2D>> curves; */
/*   for (const auto &p : paths) { */
/*     // std::cout << "Loading curve from " << p << std::endl; */
/*     auto c = load_curve_giscup(p); */
/*     curves.push_back(c); */
/*     // std::cout << "Loaded curve with " << c.points.size() << " points" << */
/*     // std::endl; */
/*   } */
/*   return curves; */
/* } */

TEST_CASE("Baseline distance function should be symmetric", "[frechet]") {
  std::vector<Curve<Point2D>> curves = load_dataset(dataset_name());
  std::cout << "Loaded " << curves.size() << " curves" << std::endl;

  std::vector<double> row1;
  std::vector<double> row2;

  for (size_t i = 0; i < curves.size(); i++) {
    for (size_t j = i + 1; j < curves.size(); j++) {
      std::vector<Point2D> a = curves[i].points;
      std::vector<Point2D> b = curves[j].points;
      REQUIRE(discrete_frechet_distance_r(a, b) == discrete_frechet_distance_r(b, a));
    }
  }
}

TEST_CASE("Distance function should be symmetric", "[frechet]") {
  std::vector<Curve<Point2D>> curves = load_dataset(dataset_name());
  std::cout << "Loaded " << curves.size() << " curves" << std::endl;

  std::vector<double> row1;
  std::vector<double> row2;

  for (size_t i = 0; i < curves.size(); i++) {
    for (size_t j = i + 1; j < curves.size(); j++) {
      auto a = curves[i];
      auto b = curves[j];
      REQUIRE(discrete_frechet_distance(a.points, b.points, row1, row2) ==
              discrete_frechet_distance(b.points, a.points, row1, row2));
    }
  }
}

TEST_CASE("Comparison with the baseline", "[frechet]") {
  std::vector<Curve<Point2D>> curves = load_dataset(dataset_name());
  std::cout << "Loaded " << curves.size() << " curves" << std::endl;
  size_t base_id = curves[0].id;
  auto base_points = curves[0];

  std::vector<double> row1;
  std::vector<double> row2;

  for (size_t i = 0; i < curves.size(); i++) {
    auto pts = curves[i];
    double expected = discrete_frechet_distance_r(base_points.points, pts.points);
    double actual = discrete_frechet_distance(base_points.points, pts.points, row1, row2);
    REQUIRE(expected == actual);
  }
}

TEST_CASE("Comparison with bounded variant (infty)", "[frechet]") {
  std::vector<Curve<Point2D>> curves = load_dataset(dataset_name());
  std::cout << "Loaded " << curves.size() << " curves" << std::endl;
  size_t base_id = curves[0].id;
  auto base_points = curves[0];

  std::vector<double> row1;
  std::vector<double> row2;

  double bound = std::numeric_limits<double>::infinity();

  for (size_t i = 0; i < curves.size(); i++) {
    auto pts = curves[i];
    double expected = discrete_frechet_distance(base_points.points, pts.points, row1, row2);
    double actual = discrete_frechet_distance(base_points, pts, bound, row1, row2);
    REQUIRE(expected == actual);
  }
}

TEST_CASE("Comparison with bounded variant (expected)", "[frechet]") {
  std::vector<Curve<Point2D>> curves = load_dataset(dataset_name());
  std::cout << "Loaded " << curves.size() << " curves" << std::endl;
  size_t base_id = curves[0].id;
  auto base_points = curves[0];

  std::vector<double> row1;
  std::vector<double> row2;

  for (size_t i = 0; i < curves.size(); i++) {
    auto pts = curves[i];
    double expected = discrete_frechet_distance(base_points.points, pts.points, row1, row2);
    double bound = expected;
    double actual = discrete_frechet_distance(base_points, pts, bound, row1, row2);
    REQUIRE(expected == actual);
  }
}

TEST_CASE("Test predicate variant", "[frechet]") {
  std::vector<Curve<Point2D>> curves = load_dataset(dataset_name());
  std::cout << "Loaded " << curves.size() << " curves" << std::endl;

  std::vector<double> row1;
  std::vector<double> row2;
  std::vector<bool> visited;
  std::vector<size_t> stack;

  for (size_t i = 0; i < curves.size(); i++) {
    for (size_t j = i + 1; j < curves.size(); j++) {
      auto a = curves[i];
      auto b = curves[j];
      double dist = discrete_frechet_distance(a.points, b.points, row1, row2);
      REQUIRE(discrete_frechet_distance_predicate(a, b, dist, stack, visited));
      REQUIRE(discrete_frechet_distance_predicate(a, b, 2 * dist, stack, visited));
      if (dist > 0) {
        REQUIRE(!discrete_frechet_distance_predicate(a, b, 0.99 * dist, stack, visited));
      }
    }
  }
}

TEST_CASE("Continuous distance bounds - discrete", "[continuous]") {
  auto rnd = std::mt19937{std::random_device{}()};
  std::vector<Curve<Point2D>> a, b, curves = load_dataset(dataset_name());
  std::shuffle(curves.begin(), curves.end(), rnd);

  std::vector<double> tmp1, tmp2;

  size_t n_curves = std::min(30ul, curves.size()/2ul);
  
  for (size_t i=0; i<n_curves; i++) {
    a.push_back(curves[i]);
  }
  for (size_t i=n_curves; i<n_curves*2; i++) {
    b.push_back(curves[i]);
  }
  for (const auto & curve_a : a) {
    for (const auto & curve_b : b) {
      double discrete_dist = discrete_frechet_distance(curve_a.points, curve_b.points, tmp1, tmp2);
      std::cout << "A: " << curve_a.id << " B: " << curve_b.id << " Discrete=" << discrete_dist << std::endl;
      REQUIRE(continuous_frechet_distance_predicate(curve_a, curve_b, discrete_dist+0.0001)); // The 0.0001 is to take into account numerical instabilities
    }
  }
}

TEST_CASE("Continuous distance bounds - equal time", "[continuous]") {
  auto rnd = std::mt19937{std::random_device{}()};
  std::vector<Curve<Point2D>> a, b, curves = load_dataset(dataset_name());
  std::shuffle(curves.begin(), curves.end(), rnd);

  std::vector<double> tmp1, tmp2;

  size_t n_curves = 30;
  
  for (size_t i=0; i<n_curves; i++) {
    a.push_back(curves[i]);
  }
  for (size_t i=n_curves; i<n_curves*2; i++) {
    b.push_back(curves[i]);
  }
  std::cout << "=======================" << std::endl;
  for (const auto & curve_a : a) {
    for (const auto & curve_b : b) {
      double et_dist = equal_time_distance(curve_a, curve_b);
      std::cout << "A: " << curve_a.id << " B: " << curve_b.id << " ET=" << et_dist << std::endl;
      REQUIRE(continuous_frechet_distance_predicate(curve_a, curve_b, et_dist));
    }
  }
}

TEST_CASE("Continuous distance", "[targeted]") {
  Curve<Point2D> a, b;
  a.push_back(Point2D{-13614796.74, 4561886.883});
  a.push_back(Point2D{-13613795.98, 4557859.909});
  a.push_back(Point2D{-13613469.81, 4556413.625});
  
  b.push_back(Point2D{-13612634.91, 4558828.44});
  b.push_back(Point2D{-13611790, 4557145.199});

  std::vector<double> tmp1, tmp2;
  double discrete = discrete_frechet_distance(a.points, b.points, tmp1, tmp2);
  double et = equal_time_distance(a, b);
  printf("discrete Frechet distance: %f equal time distance: %f\n", discrete, et);
  REQUIRE(continuous_frechet_distance_predicate(a, b, et));
  REQUIRE(continuous_frechet_distance_predicate(a, b, discrete));
}
