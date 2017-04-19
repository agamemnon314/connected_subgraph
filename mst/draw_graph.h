#ifndef DRAW_GRAPH_H
#define DRAW_GRAPH_H
#include <lemon/dim2.h>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

typedef ListGraph::Node Node;
typedef ListGraph::NodeIt NodeIt;
typedef ListGraph::Edge Edge;
typedef ListGraph::EdgeIt EdgeIt;
typedef dim2::Point<double> Point;

void draw_graph(const ListGraph &g, const ListGraph::NodeMap<Point> &coords,
                const ListGraph::EdgeMap<bool> &edge_flag) {
  ListGraph::NodeMap<Color> colors(g);
  ListGraph::NodeMap<double> sizes(g);

  ListGraph::EdgeMap<double> ewidths(g);

  IdMap<ListGraph, Node> id(g);

  for (NodeIt u(g); u != INVALID; ++u) {
    colors[u] = RED;
    sizes[u] = 30;
  }
  for (EdgeIt e(g); e != INVALID; ++e) {
    if (edge_flag[e]) {
      ewidths[e] = 20;
    } else {
      ewidths[e] = 0;
    }
  }

  graphToEps(g, "fuckyou.eps")
      .coords(coords)
      .nodeColors(colors)
      .absoluteNodeSizes()
      .nodeSizes(sizes)
      .nodeTexts(id)
      .absoluteEdgeWidths()
      .edgeWidths(ewidths)
      .run();
}

#endif
