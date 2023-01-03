// graph.h <Starter Code>
// < Liwo Huang >
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// University of Illinois at Chicago
// CS 251: Fall 2021
// Project #7 - Openstreet Maps
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>

#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:
  int numVertices;
  int numEdges;
  vector<VertexT> all_Vertices;
  typedef unordered_map<VertexT, WeightT> vwpair;
  unordered_map<VertexT, vwpair> adjList;

 public:

  //
  // constructor:
  //
  // Constructs an empty graph where n is the max # of vertices
  // you expect the graph to contain.
  //
  // NOTE: the graph is implemented using an adjacency matrix.
  // If n exceeds the dimensions of this matrix, an exception
  // will be thrown to let you know that this implementation
  // will not suffice.
  //
  graph() {
    numVertices = 0;
    numEdges = 0;
  }
  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const {
    return numVertices;
  }
  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const {
    return numEdges;
  }
  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v) {
    if (this->adjList.count(v) == true) {
      return false;
    } else {
      unordered_map<VertexT, WeightT> emp;
      this->adjList.emplace(v, emp);
      this->numVertices++;
      this->all_Vertices.push_back(v);
      return true;
    }
  }
  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    if (this->adjList.count(from) == true && this->adjList.count(to) == true) {
      if (this->adjList.at(from).count(to) == true) {
        this->adjList.at(from).at(to) = weight;
        return true;
      } else {
        this->adjList.at(from).emplace(to, weight);
        this->numEdges++;
        return true;
      }
    } else {
      return false;
    }
  }
  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
    if (this->adjList.count(from) == true && this->adjList.count(to) == true) {
      if (this->adjList.at(from).count(to) == true) {
        weight =  this->adjList.at(from).at(to);
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT> S;
    if (this->adjList.count(v) == false) {
      return S;
    } else if (this->adjList.at(v).empty()) {
      return S;
    } else {
      for (auto t : this->adjList.at(v)) {
        S.insert(t.first);
      }
      return S;
    }
  }
  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const {
    // vector<VertexT> temp;
    // temp = this->adjList.first;
    // for (auto f:this->adjList) {
    //   temp.push_back(f.first);
    // }
    return this->all_Vertices;  // returns a copy:
  }
  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  // graph<string,int>  G(26);
  // ...
  // G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const {
    
    // graph<string,int> G;
  }
};