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
#include "types.h"
#include "io.h"
#include <boost/program_options.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include "rand.h"
#include <random>

int main(int argc, char** argv) {
  namespace po = boost::program_options;
  if (argc == 1) {
    std::cerr << "Should provide one amoung the following subcommands"
              << std::endl << std::endl
              << "convert" << std::endl
              << "genqueries" << std::endl
              << "sample" << std::endl
              << "info" << std::endl
              << "query-info" << std::endl
              << std::endl;
    return 1;
  }

  std::string subcommand(argv[1]);

  if (subcommand == "convert") {
    po::options_description opts("Global options");
    opts.add_options()
      ("input", po::value<std::string>()->required(), "The input path")
      ("output", po::value<std::string>()->required(), "The output path")
      ("from,f", po::value<std::string>()->default_value("giscup"), "Input format")
      ("to,t", po::value<std::string>()->default_value("msgpack"), "Output format")
      ("help", "Print this help message");
    
    po::positional_options_description pos;
    pos.add("input", 1).add("output", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc - 1, argv + 1)
                                    .options(opts)
                                    .positional(pos)
                                    .run();
    po::store(parsed, vm);
    if (vm.count("help") || vm.count("input") == 0 || vm.count("output") == 0) {
      std::cerr << opts << std::endl;
      return 1;
    }
    po::notify(vm);
    std::string input = vm["input"].as<std::string>();
    std::string output = vm["output"].as<std::string>();
    std::string input_format = vm["from"].as<std::string>();
    std::string output_format = vm["to"].as<std::string>();

    std::cout << "Loading input" << std::endl;
    std::vector<Curve<Point2D>> dataset;
    if (input_format == "giscup") {
      dataset = load_dataset_giscup(input);
    } else if (input_format == "msgpack") {
      dataset = load_dataset_msgpack<Point2D>(input);
    } else {
      std::cerr << "Unknown input format " << input_format;
      return 2;
    }
    
    std::cout << "Writing output" << std::endl;
    if (output_format == "giscup") {
      std::cerr << output_format << " not yet supported" << std::endl;
      return 3;
    } else if (output_format == "msgpack") {
      if (!boost::algorithm::ends_with(output, ".bz2")) {
        output += ".bz2";
      }
      std::ofstream out_stream(output, std::ios_base::out);
      boost::iostreams::filtering_ostream outs;
      outs.push(boost::iostreams::bzip2_compressor());
      outs.push(out_stream);
      for (const auto & c : dataset) {
        msgpack::pack(outs, c);
      }
      //msgpack::pack(outs, dataset);
    } else {
      std::cerr << "Unknown output format " << output_format;
      return 2;
    }
    std::cout << "Done!" << std::endl;
  } else if (subcommand == "sample") {

    po::options_description opts("Global options");
    opts.add_options()
      ("input", po::value<std::string>()->required(), "The input path")
      ("output", po::value<std::string>()->required(), "The output path")
      ("num", po::value<size_t>()->required(), "The number of elements to sample")
      ("help", "Print this help message");
    
    po::positional_options_description pos;
    pos.add("input", 1).add("output", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc - 1, argv + 1)
                                    .options(opts)
                                    .positional(pos)
                                    .run();
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cerr << opts << std::endl;
      return 1;
    }

    std::string input = vm["input"].as<std::string>();
    std::string output = vm["output"].as<std::string>();

    std::cout << "Loading input" << std::endl;
    std::vector<Curve<Point2D>> dataset = load_dataset(input);

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(dataset.begin(), dataset.end(), g);

    size_t num_output = std::min(vm["num"].as<size_t>(), dataset.size());

    std::cout << "Writing output" << std::endl;
    if (!boost::algorithm::ends_with(output, ".bz2")) {
      output += ".bz2";
    }
    std::ofstream out_stream(output, std::ios_base::out);
    boost::iostreams::filtering_ostream outs;
    outs.push(boost::iostreams::bzip2_compressor());
    outs.push(out_stream);
    for (size_t i=0; i<num_output; i++) {
      msgpack::pack(outs, dataset[i]);
    }

    std::cout << "Done!" << std::endl;

  } else if (subcommand == "info") {
    if (argc != 3) {
      std::cout << "A path to a dataset is needed" << std::endl;
    }
    std::string path = argv[2];
    std::vector<Curve<Point2D>> dataset = load_dataset(path);

    size_t
      min_len=std::numeric_limits<size_t>::max(),
      max_len=0,
      avg_len=0,
      num_curves=0;
    for(const auto & c : dataset) {
      size_t l = c.points.size();
      min_len = std::min(min_len, l);
      max_len = std::max(max_len, l);
      avg_len += l;
      num_curves++;
    }
    avg_len = avg_len / num_curves;

    std::cout << "Dataset " << path
              << std::endl << std::endl
              << " Number of curves " << dataset.size() << std::endl
              << " Average length " << avg_len << std::endl
              << " Minimum length " << min_len << std::endl
              << " Maximum length " << max_len << std::endl;
  }

  return 0;
}
