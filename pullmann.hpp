// Authors: Max Kimmelman (mkimmelm@u.rochester.edu)
//			Alan Chiu (pchiu4@u.rochester.edu)

#include <cstdlib>
#include <ctime>
#include <climits>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <thread>
#include <chrono>
#include <cstring>
#include <string>
#include <mutex>

using namespace std;


class Vertex;
struct weight_vertex;
class Edge;
class Graph;



class Vertex{
public:
	int id;
	int degree;
	vector<Edge> incidentSet;

	Vertex();
	Vertex(int id);
	vector<int> neighbors();


	bool operator<(const Vertex& v2) const;
	bool operator==(const Vertex& v2) const;
	bool operator>(const Vertex& v2) const;
};


class Edge{
public:
	Vertex* v1; 
	Vertex* v2;

	Edge();
	Edge(Vertex* vertex1, Vertex* vertex2);
	Vertex* other(Vertex v) const;
	
};


class Graph{
public: 
	vector<Vertex> vertices;
	vector<Edge> edges;
	Graph();
	void addVertex(Vertex v);
	void addEdge(Edge e);
	Vertex findIndex(int index);

};

bool ullmann_spawn(Graph& gA, Graph& gB, bool* carray, vector<pair<int, int> > work_split);
bool ullmann_descent(Graph& gA, Graph& gB, bool* carray);
bool ullmann(Graph& gA, Graph& gB);