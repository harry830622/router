#include "vertex.hpp"

using namespace std;

const vector<int>& Vertex::edge_ids() const { return edge_ids_; }

void Vertex::ConnectEdge(int edge_id) { edge_ids_.push_back(edge_id); }
