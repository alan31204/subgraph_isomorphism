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
#include "ullmann.hpp"

using namespace std;

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

	for(int i = 0 ; i< numVertices;i++){
		Vertex v = Vertex(i);
		graph.vertices.push_back(v);
	}
	// vector starts from 0 with number 1
	int v1; 
	int v2;
	int w;
	while(!input.eof()){
		input >> v1 >> v2 >> w;
		Vertex* vertex1 = &graph.vertices[v1];
		Vertex* vertex2 = &graph.vertices[v2];
		Edge e = Edge(vertex1, vertex2);
		graph.addEdge(e);
	}
	return graph;
}

// returns true if gA is a subgraph of gB and false otherwise
bool ullmann(Graph& gA, Graph& gB){
	int vnumA = gA.vertices.size();
	int deg;
	unordered_set<int> *carray[vnumA], ncandidates;
	vector<int> cneighbors;
	Vertex v;
	bool disjoint;

	// initialize candidate sets for each vertex in gA and begin adding candidates with
	//   some primary pruning
	for(int i = 0;i < vnumA;i++){
		carray[i] = new unordered_set<int>();

		// adds vertex to candidate set if has greater or equal degree
		deg = gA.findIndex(i).degree;
		for(auto& v : gB.vertices){
			if(deg <= v.degree)
				(*carray[i]).insert(v.id);
		}
	}


	cout << "primary pruning" << endl;
	for(int i = 0;i < vnumA;i++){
		cout << i + 1 << ": ";
		for(int c : *carray[i])
			cout << c + 1 << " ";
		cout << endl;
	}


	// secondary pruning
	for(int i = 0;i < vnumA;i++){						// iterate over each vertex id i in gA
		v = gA.findIndex(i);							// find vertex v with id i
		for(auto& c : *carray[i]){						// iterate over candidates for vertex
			cneighbors = gB.findIndex(c).neighbors();	// find neighbors of selected candidate
			for(int n : v.neighbors()){					// iterate over neighbors of v and 
				disjoint = true;						//   determine if cneighbors and
														//   v.neighbors() are disjoint
				for(int cn : cneighbors){
					ncandidates = *carray[n];
					if(ncandidates.find(cn) != ncandidates.end()){
						disjoint = false;
						break;
					}
				}

				if(disjoint){					// if cneighbors and v.neighbors() are disjoint
					(*carray[i]).erase(c);		//   then c must be removed from v's candidate
					break;						//   set, and then we can check v's next 
				}								//   candidate, else, check for next neighbor
			}
		}
	}

	cout << "\n\nsecondary pruning\n" << endl;
	for(int i = 0;i < vnumA;i++){
		cout << i + 1 << ": ";
		for(int c : *carray[i])
			cout << c + 1 << " ";
		cout << endl;
	}

	// check if isomorphism is still possible after pruning
	for(int i = 0;i < vnumA;i++)
		if((*carray[i]).empty()) return false;



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

	bool result = ullmann(ref(graphA), ref(graphB));
	cout << "graph A is a subgraph of graph B? " << result << endl;

	return 0;
}


