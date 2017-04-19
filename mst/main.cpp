#include "draw_graph.h"
#include "random_complete_graph.h"
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <lemon/list_graph.h>

using namespace std;

int main(int argc, char const *argv[]) {
  ListGraph g; //定义无向图G
  typedef ListGraph::Node Node;
  typedef ListGraph::NodeIt NodeIt;
  typedef ListGraph::Edge Edge;
  typedef ListGraph::EdgeIt EdgeIt;
  ListGraph::EdgeMap<double> edge_cost(g);
  ListGraph::NodeMap<dim2::Point<double>> coords(g);
  int n = 30;
  random_complete_graph(g, edge_cost, coords, n);
  cout << "Let's fight!" << endl;
  cout << countNodes(g) << endl;
  cout << countEdges(g) << endl;
  IloEnv env;

  IloModel model(env);

  ListGraph::EdgeMap<IloBoolVar> x(g);
  for (EdgeIt e(g); e != INVALID; ++e) {
    x[e] = IloBoolVar(env);
  }
  IloExpr obj(env);
  for (EdgeIt e(g); e != INVALID; ++e) {
    obj += edge_cost[e] * x[e];
  }
  model.add(IloMinimize(env, obj));

  IloExpr expr(env);
  for (EdgeIt e(g); e != INVALID; ++e) {
    expr += x[e];
  }
  model.add(expr == countNodes(g) - 1);

  obj.end();

  IloCplex cplex(model);
  cplex.solve();

  env.out() << "Cost:" << cplex.getObjValue() << endl;

  ListGraph::EdgeMap<bool> edge_flag(g);
  for (EdgeIt e(g); e != INVALID; ++e) {
    edge_flag[e] = cplex.getValue(x[e]);
    if (edge_flag[e]) {
      cout << g.id(g.u(e)) << "--" << g.id(g.v(e)) << endl;
    }
  }
  draw_graph(g, coords, edge_flag);

  return 0;
}
