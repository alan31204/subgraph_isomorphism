// Authors: Max Kimmelman (mkimmelm@u.rochester.edu)
//			Alan Chiu (pchiu4@u.rochester.edu)

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
#include <fstream>
#include <unordered_set>
#include "pullmann.hpp"

using namespace std;

#define RIDX(i, j, n) (i * n + j)

Vertex::Vertex(){

}

Vertex::Vertex(int id){
	this->id = id;
	this->degree = 0;
	this->incidentSet = vector<Edge>();
}

// returns neighbors as vector of Vertex ids
vector<int> Vertex::neighbors(){
	vector<int> neighbors = vector<int>();

	for(auto& e : this->incidentSet)
		neighbors.push_back(e.other(*this)->id);

	return neighbors;
}

// Special design to make priority queue for minimum by changing < and >
bool Vertex::operator<(const Vertex& v2) const{
	return this->degree < v2.degree;
}

// Special design to make priority queue for minimum
bool Vertex::operator>(const Vertex& v2) const{
	return this->degree > v2.degree;
}

bool Vertex::operator==(const Vertex& v2) const{
	return this->degree == v2.degree;
}


Edge::Edge(){

}

Edge::Edge(Vertex* vertex1, Vertex* vertex2){
	this->v1 = vertex1; 
	this->v2 = vertex2;
}

Vertex* Edge::other(Vertex v) const{
	if(this->v1->id == v.id){
		return this->v2;
	}else if(this->v2->id == v.id){
		return this->v1;
	}else{
		exit(1);
	}
}


Graph::Graph(){

}

void Graph::addVertex(Vertex v){
	this->vertices.push_back(v);
}


void Graph::addEdge(Edge e){
	e.v1->incidentSet.push_back(e);
	e.v2->incidentSet.push_back(e);
	e.v1->degree++;
	e.v2->degree++;
	this->edges.push_back(e);
}


Vertex Graph::findIndex(int index){
	return this->vertices[index];
}

Graph loadGraph(string filename){
	Graph graph;
	ifstream input(filename);

	int numVertices;
	int numEdges;

	input >> numVertices;
	input >> numEdges;

	for(int i = 0 ; i < numVertices;i++){
		Vertex v = Vertex(i);
		graph.vertices.push_back(v);
	}
	// vector starts from 0 with number 1
	int v1, v2, w, edgesAdded = 0;
	while(!input.eof() && edgesAdded < numEdges){
		input >> v1 >> v2 >> w;
		Vertex* vertex1 = &graph.vertices[v1];
		Vertex* vertex2 = &graph.vertices[v2];
		Edge e = Edge(vertex1, vertex2);
		graph.addEdge(e);
		edgesAdded++;
	}
	return graph;
}

int vnumA;
int vnumB;

bool ullmann_descent(Graph& gA, Graph& gB, bool* carray){
	
	return false;
}

// returns true if gA is a subgraph of gB and false otherwise
bool ullmann(Graph& gA, Graph& gB){
	
	return false;
}




int main(int argc, char* argv[]){
	string fileA = "graphs/A";
	string fileB = "graphs/B";

	for(int i = 1;i < argc;i++){
		if(strcmp(argv[i], "-gA") == 0 && i + 1 < argc){
			fileA = (argv[i + 1]);
		}
		else if(strcmp(argv[i], "-gB") == 0 && i + 1 < argc){
			fileB = (argv[i + 1]);
		}
	}

	Graph graphA, graphB;
	graphA = loadGraph(fileA);
	graphB = loadGraph(fileB);

	// bool result = ullmann(ref(graphA), ref(graphB));
	cout << "graph A is a subgraph of graph B? " << result << endl;

	return 0;
}


