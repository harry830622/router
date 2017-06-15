#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "database.hpp"
#include "graph.hpp"
#include "line.hpp"

class Router {
 public:
  Router(Database& database);

  void DrawSpanningGraph(std::ostream& os) const;
  void DrawMinimumSpanningTree(std::ostream& os) const;
  void DrawRectilinearMinimumSpanningTree(std::ostream& os) const;

  void Output(std::ostream& os) const;

  void Run();

 private:
  void ConstructSpanningGraph();
  void ConstructMinimumSpanningTree();
  void RectilinearizeMinimumSpanningTree();

  Database& database_;
  Graph spanning_graph_;
  Graph minimum_spanning_tree_;
  std::vector<Line> horizontal_lines_;
  std::vector<Line> vertical_lines_;
};

#endif
