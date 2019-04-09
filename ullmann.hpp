#include <cstdlib>
#include <ctime>
#include <climits>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <string>
#include <mutex>

using namespace std;


class Vertex;


class Vertex{
public:
	int id;
	int degree;
	vector<Edge> incidentSet;

	Vertex();
	Vertex(int id);


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