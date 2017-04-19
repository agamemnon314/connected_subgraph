#include "draw_graph.h"
#include "random_complete_graph.h"
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <lemon/connectivity.h>
#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/nagamochi_ibaraki.h>
using namespace std;

ILOSTLBEGIN

typedef ListGraph::Node Node;
typedef ListGraph::NodeIt NodeIt;
typedef ListGraph::Edge Edge;
typedef ListGraph::EdgeIt EdgeIt;

ILOUSERCUTCALLBACK4(SubtourEliminationUserCallback, const ListGraph &, g,
                    ListGraph::EdgeMap<IloBoolVar> &, x, IloRangeArray &,
                    lazypool, IloCplex &, cplex) {
  IloEnv env = getEnv();
  if (!isAfterCutLoop())
    // env.out() << "fuck!" << endl;
    return;
  if (lazypool.getSize() > 0) {
    int n_lazy = lazypool.getSize();
    for (size_t i = 0; i < n_lazy; i++) {
      add(lazypool[i]);
    }
    lazypool.clear();
    // env.out() << "fuck!" << endl;
  }

  // ListGraph h;
  // ListGraph::NodeMap<Node> nr(g);
  // ListGraph::EdgeMap<Edge> er(g);
  ListGraph::EdgeMap<double> cap(g);
  ListGraph::NodeMap<bool> cut(g);
  // for (NodeIt u(g); u != INVALID; ++u) {
  //   nr[u] = h.addNode();
  // }
  for (EdgeIt e(g); e != INVALID; ++e) {
    // er[e] = h.addEdge(nr[g.u(e)], nr[g.v(e)]);
    cap[e] = getValue(x[e]);
  }
  NagamochiIbaraki<ListGraph, ListGraph::EdgeMap<double>> ni(g, cap);
  ni.run();
  ni.minCutMap(cut);

  if (ni.minCutValue() < 2 - 0.1) {
    cout << "nani??" << endl;
    IloExpr expr(env);
    for (EdgeIt e(g); e != INVALID; ++e) {
      if (cut[g.u(e)] != cut[g.v(e)]) {
        expr += x[e];
      }
    }
    add(expr >= 2);
    expr.end();
  }
  // for (NodeIt u(g); u != INVALID; ++u) {
  // }
  return;
}

ILOLAZYCONSTRAINTCALLBACK4(SubtourEliminationLazyCallback, const ListGraph &, g,
                           ListGraph::EdgeMap<IloBoolVar> &, x, IloRangeArray &,
                           lazypool, IloCplex &, cplex) {
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
    IloExprArray exprs(env, n_comp);
    for (int i = 0; i < n_comp; i++) {
      exprs[i] = IloExpr(env);
    }

    // cout << n_comp << endl;
    // cout << exprs.getSize() << endl;
    for (EdgeIt e(g); e != INVALID; ++e) {
      if (comp[nr[g.u(e)]] != comp[nr[g.v(e)]]) {
        // cout << comp[nr[g.u(e)]] << endl;
        exprs[comp[nr[g.u(e)]]] += x[e];
        exprs[comp[nr[g.v(e)]]] += x[e];
      }
    }
    for (int i = 0; i < n_comp; i++) {
      add(exprs[i] >= 2);
      lazypool.add(exprs[i] >= 2);
      exprs[i].end();
    }
    exprs.end();
    // cout << "fuck!" << endl;
  }

  h.clear();

  return;
}

int main(int argc, char const *argv[]) {
  ListGraph g; //定义无向图G
  ListGraph::EdgeMap<double> edge_cost(g);
  ListGraph::NodeMap<dim2::Point<double>> coords(g);
  int n = 200;
  random_complete_graph(g, edge_cost, coords, n);

  // ListGraph::EdgeMap<int> x_edge(g);

  // cplex
  IloEnv env;
  try {

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
    model.add(expr == countNodes(g));
    expr.clear();

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
    cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                   IloCplex::Traditional);

    IloRangeArray lazypool(env);
    cplex.use(SubtourEliminationUserCallback(env, g, x, lazypool, cplex));
    cplex.use(SubtourEliminationLazyCallback(env, g, x, lazypool, cplex));

    cplex.solve();

    env.out() << "Cost:" << cplex.getObjValue() << endl;

    ListGraph::EdgeMap<bool> edge_flag(g);
    for (EdgeIt e(g); e != INVALID; ++e) {
      edge_flag[e] = cplex.getValue(x[e]);
      // if (edge_flag[e]) {
      //   cout << g.id(g.u(e)) << "--" << g.id(g.v(e)) << endl;
      // }
    }
    draw_graph(g, coords, edge_flag);
    env.end();
  } catch (const IloException &e) {
    cerr << "Exception caught: " << e << endl;
  } catch (...) {
    cerr << "Unknown exception caught!" << endl;
  }
  return 0;
}
