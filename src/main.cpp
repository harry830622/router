#include "router.hpp"

#include <boost/program_options.hpp>

#include <fstream>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {
  po::options_description options("Options");

  // clang-format off
  options.add_options()
    ("help,h", "Print help messages")
    ("draw-spanning-graph,g", po::value<string>()->value_name("FILE"), "Draw spanning graph")
    ("draw-minimum-spanning-tree,t", po::value<string>()->value_name("FILE"), "Draw minimum spanning tree")
    ("input", po::value<string>()->value_name("FILE")->required(), "Input")
    ("output", po::value<string>()->value_name("FILE")->required(), "Output")
    ;
  // clang-format on

  po::positional_options_description positional_options;
  positional_options.add("input", 1);
  positional_options.add("output", 1);

  po::variables_map arguments;

  po::store(po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .run(),
            arguments);

  if (arguments.count("help") || argc < 3) {
    cout << "Usage:" << endl;
    cout << "  " << argv[0] << " [--draw-spanning-graph <file>]"
         << " <input> <output>" << endl;
    cout << options;
    return 0;
  }

  const string input_name = arguments["input"].as<string>();

  ifstream input(input_name);

  Database database(input);
  /* database.Print(); */

  Router router(database);
  router.Run();

  if (arguments.count("draw-spanning-graph")) {
    ofstream plot(arguments["draw-spanning-graph"].as<string>());
    router.DrawSpanningGraph(plot);
  }

  if (arguments.count("draw-minimum-spanning-tree")) {
    ofstream plot(arguments["draw-minimum-spanning-tree"].as<string>());
    router.DrawMinimumSpanningTree(plot);
  }

  return 0;
}
