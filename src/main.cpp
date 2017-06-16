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
    ("dsg,g", po::value<string>()->value_name("FILE"), "Draw spanning graph")
    ("dmst,t", po::value<string>()->value_name("FILE"), "Draw minimum spanning tree")
    ("drmst,r", po::value<string>()->value_name("FILE"), "Draw rectilinear minimum spanning tree")
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
    cout << "  " << argv[0] << " [options]"
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

  const string output_name = arguments["output"].as<string>();
  ofstream output(output_name);
  router.Output(output);

  if (arguments.count("dsg")) {
    ofstream plot(arguments["dsg"].as<string>());
    router.DrawSpanningGraph(plot);
  }

  if (arguments.count("dmst")) {
    ofstream plot(arguments["dmst"].as<string>());
    router.DrawMinimumSpanningTree(plot);
  }

  if (arguments.count("drmst")) {
    ofstream plot(arguments["drmst"].as<string>());
    router.DrawRectilinearMinimumSpanningTree(plot);
  }

  return 0;
}
