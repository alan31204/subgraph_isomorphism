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
	vector<int> cneighbors;
	Vertex v;
	bool disjoint, solved, rcarray[vnumA][vnumB];
	int numCandidates[vnumA], numCandidatesFor[vnumB];

	// secondary pruning
	for(int i = 0;i < vnumA;i++){						// iterate over each vertex id i in gA
		v = gA.findIndex(i);							// find vertex v with id i
		for(int c = 0;c < vnumB;c++){					// iterate over candidates for vertex
			if(!carray[RIDX(i, c, vnumB)]) continue;	//   by skipping over noncandidates
			cneighbors = gB.findIndex(c).neighbors();	// find neighbors of selected candidate
			for(int n : v.neighbors()){					// iterate over neighbors of v and 
				disjoint = true;						//   determine if cneighbors and
														//   v.neighbors() are disjoint
				for(int cn : cneighbors){
					if(carray[RIDX(n, cn, vnumB)]){
						disjoint = false;
						break;
					}
				}

				// if cneighbors and v.neighbors() are disjointthen c must be removed from v's
				//   candidate set, and then we can check v's next candidate, else, check for
				//   next neighbor
				if(disjoint){
					carray[RIDX(i, c, vnumB)] = false;
					break;
				}
			}
		}
	}

	for(int i = 0;i < vnumA;i++) numCandidates[i] = 0;
	for(int i = 0;i < vnumA;i++){
		for(int c  = 0;c < vnumB;c++)
			numCandidates[i] += (int) carray[RIDX(i, c, vnumB)];
	}
	for(int c = 0;c < vnumB;c++) numCandidatesFor[c] = 0;
	for(int c = 0;c < vnumB;c++){
		for(int i = 0;i < vnumA;i++)
			numCandidatesFor[c] += (int) carray[RIDX(i, c, vnumB)];
	}

	solved = true;
	for(int i = 0;i < vnumA;i++){
		if(numCandidates[i] == 0) return false;
		else if(numCandidates[i] > 1) solved = false;
	}
	for(int c = 0;c <  vnumB;c++)
		if(numCandidatesFor[c] > 1) solved = false;
	if(solved) return true;

	for(int i = 0;i < vnumA;i++){
		if(numCandidates[i] == 1) continue;
		for(int c = 0;c < vnumB;c++){
			if(!carray[RIDX(i, c, vnumB)]) continue;

			memcpy(rcarray, carray, vnumA * vnumB * sizeof(bool));
			for(int x = 0;x < vnumA;x++)
				rcarray[x][c] = false;
			for(int y = 0;y < vnumB;y++)
				rcarray[i][y] = false;
			rcarray[i][c] = true;

			if(ullmann_descent(gA,gB,&rcarray[0][0])) return true;
		}
	}

	return false;
}

// returns true if gA is a subgraph of gB and false otherwise
bool ullmann(Graph& gA, Graph& gB){
	vnumA = gA.vertices.size();
	vnumB = gB.vertices.size();
	int deg, numCandidates[vnumA], numCandidatesFor[vnumB];
	bool carray[vnumA][vnumB], rcarray[vnumA][vnumB];
	vector<int> cneighbors;
	Vertex v;
	bool disjoint, solved;

	if(vnumA > vnumB) return false;

	// initialize candidate arrays while doing primary pruning and set all empty values to false
	for(int i = 0;i < vnumA;i++){
		deg = gA.findIndex(i).degree;
		for(int j = 0;j < vnumB;j++)
			carray[i][j] = (deg <= gB.findIndex(j).degree);
	}

	// secondary pruning
	for(int i = 0;i < vnumA;i++){						// iterate over each vertex id i in gA
		v = gA.findIndex(i);							// find vertex v with id i
		for(int c = 0;c < vnumB;c++){					// iterate over candidates for vertex
			if(!carray[i][c]) continue;					//   by skipping over noncandidates
			cneighbors = gB.findIndex(c).neighbors();	// find neighbors of selected candidate
			for(int n : v.neighbors()){					// iterate over neighbors of v and 
				disjoint = true;						//   determine if cneighbors and
														//   v.neighbors() are disjoint
				for(int cn : cneighbors){
					if(carray[n][cn]){
						disjoint = false;
						break;
					}
				}

				if(disjoint){					// if cneighbors and v.neighbors() are disjoint
					carray[i][c] = false;		//   then c must be removed from v's candidate
					break;						//   set, and then we can check v's next 
				}								//   candidate, else, check for next neighbor
			}
		}
	}

	for(int i = 0;i < vnumA;i++) numCandidates[i] = 0;
	for(int i = 0;i < vnumA;i++){
		for(int c  = 0;c < vnumB;c++)
			numCandidates[i] += (int) carray[i][c];
	}
	for(int c = 0;c < vnumB;c++) numCandidatesFor[c] = 0;
	for(int c = 0;c < vnumB;c++){
		for(int i = 0;i < vnumA;i++)
			numCandidatesFor[c] += (int) carray[i][c];
	}

	solved = true;
	for(int i = 0;i < vnumA;i++){
		if(numCandidates[i] == 0) return false;
		else if(numCandidates[i] > 1) solved = false;
	}
	for(int c = 0;c <  vnumB;c++)
		if(numCandidatesFor[c] > 1) solved = false;
	if(solved) return true;

	for(int i = 0;i < vnumA;i++){
		if(numCandidates[i] == 1) continue;
		for(int c = 0;c < vnumB;c++){
			if(!carray[i][c]) continue;

			memcpy(rcarray, carray, vnumA * vnumB * sizeof(bool));
			for(int x = 0;x < vnumA;x++)
				rcarray[x][c] = false; // removing chosen elements from other candidate array
			for(int y = 0;y < vnumB;y++)
				rcarray[i][y] = false; // removing elements that's not chosen in current array
			rcarray[i][c] = true;

			if(ullmann_descent(gA,gB,&rcarray[0][0])) return true;
		}
	}

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


