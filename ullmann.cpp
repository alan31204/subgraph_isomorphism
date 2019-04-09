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

Vertex::Vertex(){

}

Vertex::Vertex(int id){
	this->id = id;
	this->degree = 0;
	this->incidentSet = vector<Edge>();
}

vector<struct weight_vertex> Vertex::neighbors(){
	vector<struct weight_vertex> neighbors;

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

	for(int i = 0 ; i< numVertices;i++){
		Vertex v = Vertex(i);
		graph.vertices.push_back(v);
	}
	// vector starts from 0 with number 1
	int v1; 
	int v2;
	double w;
	while(!input.eof()){
		input >> v1 >> v2 >> w;
		Vertex* vertex1 = &graph.vertices[v1];
		Vertex* vertex2 = &graph.vertices[v2];
		// Edge e = Edge(vertex1, vertex2, w);
		Edge e = Edge(vertex1, vertex2);
		graph.addEdge(e);
	}
	return graph;
}
