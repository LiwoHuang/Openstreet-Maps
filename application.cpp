// application.cpp <Starter Code>
// <Liwo Huang>
//
// University of Illinois at Chicago
// CS 251: Fall 2021
// Project #7 - Openstreet Maps
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//
#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <queue>
#include <stack>
#include <limits>
#include <set>
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"

using namespace std;
using namespace tinyxml2;

//
// Implement your creative component application here
// TO DO: add arguments
//
const double INF = numeric_limits<double>::max();

/*
Creative Component is on line 118.
*/

BuildingInfo searchBuilding(string _query,
vector<BuildingInfo> Buildings,
bool &_found,
set<string> unreachableBuildings) {
  _found = false;
  BuildingInfo temp;
  for (auto a : Buildings) {
    if (_query == a.Abbrev) {
      if (unreachableBuildings.count(a.Fullname) == true) {
        continue;
      }
      _found = true;
      temp = a;
      return temp;
    } else if (a.Fullname.find(_query) != string::npos) {
      if (unreachableBuildings.count(a.Fullname) == true) {
        continue;
      }
      _found = true;
      temp = a;
      return temp;
    } else {
      _found = false;
    }
  }
}
BuildingInfo nearestBuilding(Coordinates midpoint,
vector<BuildingInfo> Buildings,
set<string> unreachable) {
  double min = INF;
  double temp = 0;
  BuildingInfo min_building;
  BuildingInfo temp_building;
  for (auto c : Buildings) {
    temp = distBetween2Points(midpoint.Lat,
      midpoint.Lon,
      c.Coords.Lat,
      c.Coords.Lon);
    if (temp < min) {
      if (unreachable.count(c.Fullname) == true) {
        continue;
      } else {
      min = temp;
      min_building = c;
      }
    }
  }
  return min_building;
}
long long nearestNode(BuildingInfo b,
map<long long, Coordinates> Nodes,
vector<FootwayInfo> Foot) {
  double min = INF;
  double temp = 0;
  long long nearest_node;
  for (auto f : Foot) {
    for (auto n : f.Nodes) {
      temp = distBetween2Points(b.Coords.Lat,
        b.Coords.Lon,
        Nodes.at(n).Lat,
        Nodes.at(n).Lon);
      if (temp < min) {
        min = temp;
        nearest_node = n;
      }
    }
  }
  return nearest_node;
}
/*
Creative Component:
Title: let you and your friend meet faster in the building
Description: This function is helping you and your friend can meet faster in the building.
You just only need to type you and your friend building name, and
it can automatically identify the building your and your friend located.
It will calculate the shortest distance between you and youe friend,
and find a best building you and your friend can meet faster.
*/
void creative(map<long long, Coordinates>& Nodes,
  vector<FootwayInfo>& Footways,
  vector<BuildingInfo>& Buildings,
  graph<long long, double> G) {
  string person1Building, person2Building;
  cout << endl;
  cout << "Enter what building are you right now (partial name or abbreviation), or # to quit> ";
  getline(cin, person1Building);
  while (person1Building != "#") {
    cout << "Enter what building your friend staying in (partial name or abbreviation)> ";
    getline(cin, person2Building);
    BuildingInfo building1, building2, buildingCenter;
    bool person1_found = false;
    bool person2_found = false;
    set<string> unreachableBuildings;
    // Search Buildings you and your friend
    building1 = searchBuilding(person1Building,
      Buildings,
      person1_found,
      unreachableBuildings);
    if (person1_found == false) {
      cout << "The building you said was not found" << endl;
      cout << "Enter what building are you right now (partial name or abbreviation), or # to quit> ";
      getline(cin, person1Building);
      continue;
    }
    building2 = searchBuilding(person2Building,
      Buildings, person2_found,
      unreachableBuildings);
    if (person2_found == false) {
      cout << "The building your friend said was not found" << endl;
      cout << "Enter what building are you right now (partial name or abbreviation), or # to quit> ";
      getline(cin, person1Building);
      continue;
    }
    cout << "You location:"<< endl;
    cout << " " << building1.Fullname << endl;
    cout << " (" << building1.Coords.Lat << ", "
    << building1.Coords.Lon << ")" << endl;
    cout << "Your friend location:"<< endl;
    cout << " " << building2.Fullname << endl;
    cout << " (" << building2.Coords.Lat << ", "
      << building2.Coords.Lon << ")" << endl;
    bool path_found = false;
      // Locate Center Building
      Coordinates midpoint;
      midpoint = centerBetween2Points(building1.Coords.Lat,
        building1.Coords.Lon,
        building2.Coords.Lat,
        building2.Coords.Lon);
      buildingCenter = nearestBuilding(midpoint,
        Buildings,
        unreachableBuildings);
      cout << "There is the Building your both can meet faster:"<< endl;
      cout << " " << buildingCenter.Fullname << endl;
      cout << " (" << buildingCenter.Coords.Lat << ", "
        << buildingCenter.Coords.Lon << ")" << endl;
      cout << "Have a nice meeting!" << endl;
    cout << endl;
    cout << "Enter what building are you right now (partial name or abbreviation), or # to quit> ";
    getline(cin, person1Building);
  }  
}
class prioritize {
 public:
  bool operator()(const pair<long long, double>& p1,
const pair<long long, double>& p2) const {
    if (p1.second == p2.second) {
      return p1.first > p2.first;
    }
    return p1.second > p2.second;
  }
};
map<long long, long long> DijkstraShort(graph<long long, double> G,
long long startV,
map<long long, double>& distances) {
  map<long long, long long> predecessors;
  priority_queue< pair<long long, double>,
  vector<pair<long long, double>>,
  prioritize> unvisitedQueue;
  set<long long> set_visited;
  vector<long long> vector_visited;
  for (auto currV : G.getVertices()) {
    distances[currV] = INF;
    predecessors[currV] = 0;
    unvisitedQueue.push(make_pair(currV, INF));
  }
  // startV has a distance of 0 from itself
  distances[startV] = 0;
  unvisitedQueue.push(make_pair(startV, 0));
  while (!unvisitedQueue.empty()) {
    // Visit vertex with minimum distance from startV
    long long topV = unvisitedQueue.top().first;
    unvisitedQueue.pop();
    if (distances[topV] == INF) {
      break;
    } else if (set_visited.count(topV) == true) {
      continue;
    } else {
      set_visited.insert(topV);
      vector_visited.push_back(topV);
    }
    set<long long> neighbor = G.neighbors(topV);
    for (auto adjV : neighbor) {
      double edgeWeight = 0;
      G.getWeight(topV, adjV, edgeWeight);
      double alternativePathDistance = distances[topV] + edgeWeight;
      // If shorter path from startV to adjV is found,
      // update adjV's distance and predecessor
      if (alternativePathDistance < distances[adjV]) {
        distances[adjV] = alternativePathDistance;
        predecessors[adjV] = topV;
        unvisitedQueue.push(make_pair(adjV,
        alternativePathDistance));
      }
    }
  }
  return predecessors;
}
vector<long long> getPath(map<long long, long long> prede,
long long endVertex) {
  vector<long long> vector_Vertex;
  stack<long long> my_stack;
  long long currV = endVertex;
  my_stack.push(endVertex);
  while (prede.at(currV) != 0) {
    my_stack.push(prede.at(currV));
    currV = prede.at(currV);
  }
  while (!my_stack.empty()) {
    long long curr_pop = my_stack.top();
    vector_Vertex.push_back(curr_pop);
    my_stack.pop();
  }
  return vector_Vertex;
}
//
// Implement your standard application here
// add a parameter for the graph you make.
//
void application(
  map<long long, Coordinates>& Nodes,
  vector<FootwayInfo>& Footways,
  vector<BuildingInfo>& Buildings,
  graph<long long, double> G) {
  string person1Building, person2Building;
  cout << endl;
  cout << "Enter person 1's building (partial name or abbreviation), or #> ";
  getline(cin, person1Building);
  while (person1Building != "#") {
    cout << "Enter person 2's building (partial name or abbreviation)> ";
    getline(cin, person2Building);
    //
    // lookup buildings, find nearest start and dest nodes, find center
    // run Dijkstra's alg from each start,
    // output distances and paths to destination:
    //
    // cout << "Person 1's building not found" << endl;
    // cout << "Person 2's building not found" << endl;
    BuildingInfo building1, building2, buildingCenter;
    bool person1_found = false;
    bool person2_found = false;
    long long building1_node, building2_node,
    buildingCenter_node;
    set<string> unreachableBuildings;
    // MILESTONE 7: Search Buildings 1 and 2
    building1 = searchBuilding(person1Building,
      Buildings,
      person1_found,
      unreachableBuildings);
    if (person1_found == false) {
      cout << "Person 1's building not found" << endl;
      cout << "Enter person 1's building (partial name or abbreviation), or #> ";
      getline(cin, person1Building);
      continue;
    }
    building2 = searchBuilding(person2Building,
      Buildings, person2_found,
      unreachableBuildings);
    if (person2_found == false) {
      cout << "Person 2's building not found" << endl;
      cout << "Enter person 1's building (partial name or abbreviation), or #> ";
      getline(cin, person1Building);
      continue;
    }
    cout << "Person 1's point:"<< endl;
    cout << " " << building1.Fullname << endl;
    cout << " (" << building1.Coords.Lat << ", "
    << building1.Coords.Lon << ")" << endl;
    cout << "Person 2's point:"<< endl;
    cout << " " << building2.Fullname << endl;
    cout << " (" << building2.Coords.Lat << ", "
      << building2.Coords.Lon << ")" << endl;
    bool path_found = false;
      // MILESTONE 8: Locate Center Building
      Coordinates midpoint;
      midpoint = centerBetween2Points(building1.Coords.Lat,
        building1.Coords.Lon,
        building2.Coords.Lat,
        building2.Coords.Lon);
      buildingCenter = nearestBuilding(midpoint,
        Buildings,
        unreachableBuildings);
      cout << "Destination Building:"<< endl;
      cout << " " << buildingCenter.Fullname << endl;
      cout << " (" << buildingCenter.Coords.Lat << ", "
        << buildingCenter.Coords.Lon << ")" << endl << endl;
      // MILESTONE 9: Find Nearest Nodes from buildings 1, 2 & Center
      building1_node = nearestNode(building1, Nodes, Footways);
      building2_node = nearestNode(building2, Nodes, Footways);
      buildingCenter_node = nearestNode(buildingCenter, Nodes, Footways);
      cout << "Nearest P1 node:"<< endl;
      cout << " " << building1_node << endl;
      cout << " (" << Nodes.at(building1_node).Lat << ", "
        << Nodes.at(building1_node).Lon << ")" << endl;
      cout << "Nearest P2 node:"<< endl;
      cout << " " << building2_node << endl;
      cout << " (" << Nodes.at(building2_node).Lat << ", "
        << Nodes.at(building2_node).Lon << ")" << endl;
      cout << "Nearest destination node:"<< endl;
      cout << " " << buildingCenter_node << endl;
      cout << " (" << Nodes.at(buildingCenter_node).Lat << ", "
        << Nodes.at(buildingCenter_node).Lon << ")" << endl << endl;
      // MILESTONE 10: Run Dijkstra’s Algorithm
      map<long long, long long> predecessors1;
      map<long long, long long> predecessors2;
      map<long long, double> distances1;
      map<long long, double> distances2;
      vector<long long> Path_Vector1;
      vector<long long> Path_Vector2;
      predecessors1 = DijkstraShort(G, building1_node, distances1);
      predecessors2 = DijkstraShort(G, building2_node, distances2);
      if (predecessors1.size() == 1) {
        unreachableBuildings.insert(building1.Fullname);
      }
      if (predecessors2.size() == 1) {
        unreachableBuildings.insert(building2.Fullname);
      }
      // MILESTONE 11: Print path (path found! break)
      if (distances1[building2_node] >= INF || distances2[building1_node] >= INF) {
        cout << "Sorry, destination unreachable." << endl;
        cout << "Enter person 1's building (partial name or abbreviation), or #> ";
        getline(cin, person1Building);
        continue;
      } else if (distances1[buildingCenter_node] >= INF || distances2[buildingCenter_node] >= INF) {
        cout << "At least one person was unable to reach the destination building. Finding next closest building..." << endl;
        path_found = false;
        while (path_found == false) {
          unreachableBuildings.insert(buildingCenter.Fullname);
          buildingCenter = nearestBuilding(midpoint,
            Buildings,
            unreachableBuildings);
          buildingCenter_node = nearestNode(buildingCenter,
            Nodes, Footways);
          cout << endl << "New destination building:" << endl;
          cout << " " << buildingCenter.Fullname << endl;
          cout << " (" << buildingCenter.Coords.Lat << ", "
            << buildingCenter.Coords.Lon << ")" << endl;
          cout << "Nearest destination node:" << endl;
          cout << " " << buildingCenter_node << endl;
          cout << " (" << Nodes.at(buildingCenter_node).Lat
            << ", " << Nodes.at(buildingCenter_node).Lon
            << ")"<< endl;
          cout << endl;
          path_found = true;
          if (distances1[buildingCenter_node] >= INF || distances2[buildingCenter_node] >= INF) {
            path_found = false;
            cout << "At least one person was unable to reach the destination building. Finding next closest building..." << endl;
          }
        }
        Path_Vector1 = getPath(predecessors1,
          buildingCenter_node);
        Path_Vector2 = getPath(predecessors2,
          buildingCenter_node);
        cout << "Person 1's distance to dest: "
          << distances1[buildingCenter_node] << " miles" << endl;
        cout << "Path: ";
        for (int i = 0; i < Path_Vector1.size(); i++) {
          if (i == 0) {
            cout << Path_Vector1[i];
          } else {
            cout << "->" << Path_Vector1[i];
          }
        }
        cout << endl;
        cout << "Person 2's distance to dest: "
          << distances2[buildingCenter_node] << " miles" << endl;
        cout << "Path: ";
        for (int i = 0; i < Path_Vector2.size(); i++) {
          if (i == 0) {
            cout << Path_Vector2[i];
          } else {
            cout << "->" << Path_Vector2[i];
          }
        }
        cout << endl;
      } else {
        Path_Vector1 = getPath(predecessors1, buildingCenter_node);
        Path_Vector2 = getPath(predecessors2, buildingCenter_node);
        cout << "Person 1's distance to dest: " << distances1[buildingCenter_node] << " miles" << endl;
        cout << "Path: ";
        for (int i = 0; i < Path_Vector1.size(); i++) {
          if (i == 0) {
            cout << Path_Vector1[i];
          } else {
            cout << "->" << Path_Vector1[i];
          }
        }
        cout << endl;
        cout << "Person 2's distance to dest: " << distances2[buildingCenter_node] << " miles" << endl;
        cout << "Path: ";
        for (int i = 0; i < Path_Vector2.size(); i++) {
          if (i == 0) {
            cout << Path_Vector2[i];
          } else {
            cout << "->" << Path_Vector2[i];
          }
        }
        cout << endl;
      }
    // MILESTONE 11: Find Second Nearest Destination (loop again)
    //
    // another navigation?
    //
    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);
  }
}
int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);
  string def_filename = "map.osm";
  string filename;
  cout << "Enter map filename> ";
  getline(cin, filename);
  if (filename == "") {
    filename = def_filename;
  }
  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }
  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);
  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);
  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);
  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());
  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;
  //
  // build the graph, output stats:
  //
  graph<long long, double> G;
  // MILESTONE 5: add vertices
  for (auto e : Nodes) {
    G.addVertex(e.first);
  }
  // MILESTONE 6: add edges
  for (auto e : Footways) {
    for (int i = 0; i < e.Nodes.size()-1; i++) {
      Coordinates c1 = Nodes.at(e.Nodes[i]);
      Coordinates c2 = Nodes.at(e.Nodes[i+1]);
      double distance = distBetween2Points(c1.Lat,
      c1.Lon, c2.Lat, c2.Lon);
      G.addEdge(e.Nodes[i], e.Nodes[i+1], distance);
      G.addEdge(e.Nodes[i+1], e.Nodes[i], distance);
    }
  }
  // uncomment below after MILESTONE 6
  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;
  //
  // Menu
  //
  string userInput;
  cout << "Enter \"a\" for the standard application or " << "\"c\" for the creative component application> ";
  getline(cin, userInput);
  if (userInput == "a") {
    application(Nodes, Footways, Buildings, G);
  } else if (userInput == "c") {
    // TO DO: add arguments
    creative(Nodes, Footways, Buildings, G);
  }
  //
  // done:
  //
  cout << "** Done **" << endl;
  return 0;
}
