#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "database.hpp"
#include "graph.hpp"

class Router {
 public:
  Router(Database& database);

  void DrawSpanningGraph(std::ostream& os) const;

  void Run();

 private:
  void ConstructSpanningGraph();

  Database& database_;
  Graph spanning_graph_;
};

#endif
