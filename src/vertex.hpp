#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <vector>

class Vertex {
 public:
  const std::vector<int>& edge_ids() const;

  void ConnectEdge(int edge_id);

 private:
  std::vector<int> edge_ids_;
};

#endif
