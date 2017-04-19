#ifndef RANDOM_COMPLETE_GRAPH_H
#define RANDOM_COMPLETE_GRAPH_H

#include <lemon/dim2.h>
#include <lemon/list_graph.h>
#include <cmath>
#include <random>
using namespace std;
using namespace lemon;

void random_complete_graph(ListGraph &g, ListGraph::EdgeMap<double> &edge_cost,
                           ListGraph::NodeMap<dim2::Point<double>> &coords,
                           const int n) {
  random_device rd;
  default_random_engine dre(rd());
  uniform_real_distribution<double> urd(0, 100);

  typedef ListGraph::Node Node;
  typedef ListGraph::NodeIt NodeIt;
  typedef ListGraph::Edge Edge;
  // typedef ListGraph::EdgeIt EdgeIt;
  typedef dim2::Point<double> Point;

  for (int i = 0; i < n; ++i) {
    g.addNode();
  }

  for (NodeIt u(g); u != INVALID; ++u) {
    coords[u] = Point(urd(dre), urd(dre));
  }

  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      Node u = g.nodeFromId(i), v = g.nodeFromId(j);
      Edge e = g.addEdge(u, v);
      edge_cost[e] = sqrt((coords[u] - coords[v]).normSquare());
    }
  }
}

#endif
