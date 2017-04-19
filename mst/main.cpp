#include "draw_graph.h"
#include "random_complete_graph.h"
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <lemon/connectivity.h>
#include <lemon/core.h>
#include <lemon/list_graph.h>
using namespace std;

ILOSTLBEGIN

typedef ListGraph::Node Node;
typedef ListGraph::NodeIt NodeIt;
typedef ListGraph::Edge Edge;
typedef ListGraph::EdgeIt EdgeIt;

ILOUSERCUTCALLBACK3(SubtourEliminationUserCallback, const ListGraph &, g,
                    ListGraph::EdgeMap<IloBoolVar> &, x, IloCplex &, cplex) {
  IloEnv env = getEnv();
  if (!isAfterCutLoop())
    env.out() << "fuck!" << endl;
  return;
  env.out() << "fuck!" << endl;
  for (NodeIt u(g); u != INVALID; ++u) {
  }
  return;
}

ILOLAZYCONSTRAINTCALLBACK3(SubtourEliminationLazyCallback, const ListGraph &, g,
                           ListGraph::EdgeMap<IloBoolVar> &, x, IloCplex &,
                           cplex) {
  IloEnv env = getEnv();

  ListGraph h;
  ListGraph::NodeMap<Node> nr(g);
  for (NodeIt u(g); u != INVALID; ++u) {
    nr[u] = h.addNode();
  }
  for (EdgeIt e(g); e != INVALID; ++e) {
    if (getValue(x[e]) == 1) {
      h.addEdge(nr[g.u(e)], nr[g.v(e)]);
    }
  }

  ListGraph::NodeMap<int> comp(h);
  int n_comp = 0;
  n_comp = connectedComponents(h, comp);
  if (n_comp > 1) {
    IloExpr expr(env);
    for (EdgeIt e(h); e != INVALID; ++e) {
      if (comp[h.u(e)] != comp[h.v(e)]) {
        expr += x[e];
      }
    }
    // for (NodeIt u(h); u != INVALID; ++u) {
    //   cout << comp[u] << endl;
    add(expr >= 0);

    // for (size_t i = 0; i < n_comp; i++) {
    //   add(exprs[i] >= 2).end();
    // }
    expr.end();
    cout << "fuck!" << endl;
  }

  h.clear();

  return;
}

int main(int argc, char const *argv[]) {
  ListGraph g; //定义无向图G
  ListGraph::EdgeMap<double> edge_cost(g);
  ListGraph::NodeMap<dim2::Point<double>> coords(g);
  int n = 40;
  random_complete_graph(g, edge_cost, coords, n);

  // ListGraph::EdgeMap<int> x_edge(g);

  // cplex
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
  // for (EdgeIt e(g); e != INVALID; ++e) {
  //   expr += x[e];
  // }
  // model.add(expr == countNodes(g) - 1);
  // expr.clear();

  for (NodeIt u(g); u != INVALID; ++u) {
    for (ListGraph::IncEdgeIt e(g, u); e != INVALID; ++e) {
      expr += x[e];
    }
    model.add(expr == 2);
    expr.clear();
  }

  obj.end();

  IloCplex cplex(model);
  cplex.setParam(IloCplex::Param::Preprocessing::Presolve, IloFalse);
  cplex.setParam(IloCplex::Param::Threads, 1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::Search, IloCplex::Traditional);

  // cplex.use(SubtourEliminationUserCallback(env, g, x, cplex));
  cplex.use(SubtourEliminationLazyCallback(env, g, x, cplex));

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
  env.end();
  return 0;
}
