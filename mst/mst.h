#ifndef MST_H
#define MST_H

#include <lemon/concepts/graph.h>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/math.h>
#include <iostream>
#include <random>
#include <vector>
#include "gurobi_c++.h"
using namespace std;
using namespace lemon;

class mst {
 public:
  mst();
  ~mst();
};

#endif