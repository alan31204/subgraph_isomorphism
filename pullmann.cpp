// Authors: Max Kimmelman (mkimmelm@u.rochester.edu)
//			Alan Chiu (pchiu4@u.rochester.edu)

#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <thread>
#include <chrono>
#include <cstring>
#include <string>
#include <mutex>
#include <fstream>
#include <unordered_set>
#include <cilk/cilk.h>
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


// Do the call of Cilk_spawn to split the work for different threads
bool ullmann_spawn(Graph& gA, Graph& gB, bool* carray, vector<pair<int, int>> work_split){

}


bool ullmann_descent(Graph& gA, Graph& gB, bool* carray){
	
	return false;
}

// returns true if gA is a subgraph of gB and false otherwise
bool ullmann(Graph& gA, Graph& gB){
	//Variable declaration
	vnumA = gA.vertices.size();
	vnumB = gB.vertices.size();
	int deg, numCandidates[vnumA], numCandidatesFor[vnumB];
	bool ret, carray[vnumA][vnumB], rcarray[vnumA][vnumB];
	vector<int> cneighbors;
	Vertex v;
	bool disjoint, solved;

	vector<pair<int, int>> work, work_split;

	// stop algorithm and return false if gA has more vertices than gB
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

	// add up the rows of carray to get the number of candidates per vertex in gA
	for(int i = 0;i < vnumA;i++) numCandidates[i] = 0;
	for(int i = 0;i < vnumA;i++){
		for(int c  = 0;c < vnumB;c++)
			numCandidates[i] += (int) carray[i][c];
	}

	// add up the cols of carray to get the number of times each vertex in gB is a candidate
	//   for a vertex in gA
	for(int c = 0;c < vnumB;c++) numCandidatesFor[c] = 0;
	for(int c = 0;c < vnumB;c++){
		for(int i = 0;i < vnumA;i++)
			numCandidatesFor[c] += (int) carray[i][c];
	}

	// assume carray contains a solution, then check if a row of carray is empty (return false),
	//   if a row of carray has more than one candidate left (solved = false), or if a col of
	//   carray has more than one true value (solved = false)
	solved = true;
	for(int i = 0;i < vnumA;i++){
		if(numCandidates[i] == 0) return false;			// empty row
		else if(numCandidates[i] > 1) solved = false;	// row has more than one candidate
	}
	for(int c = 0;c <  vnumB;c++)
		if(numCandidatesFor[c] > 1) solved = false;		// col has more than one true value
	
	// return true if carray contains a solution, otherwise continue recursion
	if(solved) return true;

	// pick an i in gA, c in candidates(i) where |candidates(i)| > 1 and assign i to c in
	//   rcarray (otherwise a copy of carray) and then pass rcarray to new recursive call

	work = vector<pair<int, int>>();
	for(int i = 0;i < vnumA;i++){
		if(numCandidates[i] == 1) continue; // skip vertex if already assigned
		for(int c = 0;c < vnumB;c++){
			if(!carray[i][c]) continue;
			work.push_back(make_pair(i, c));
		}
	}

	work_split = vector<pair<int, int>>();
	ret = false;
	for(int n = 0;n < work.size();n++){
		if(n < work.size() / 2){
			work_split.push_back(work[n]);
			continue;
		}

		if(n == work.size() / 2)
			ret = cilk_spawn ullmann_spawn(gA, gB, carray, work_split);

		if(ret) return true;

		// picked i and c as described above, now continue to construct rcarray
		memcpy(rcarray, carray, vnumA * vnumB * sizeof(bool));
		for(int x = 0;x < vnumA;x++)			// remove c from candidates(x) for all x in gA
			rcarray[x][work[n].second] = false;
		for(int y = 0;y < vnumB;y++)			// remove all y from candidates(i)
			rcarray[work[n].first][y] = false;
		rcarray[work[n].first][work[n].second] = true;	// re-add c to candidates(i) as sole member

		// recursively call ullman with rcarray
		if(ullmann_descent(gA,gB,&rcarray[0][0])) return true;
	}

	// if no recursive call finds an isomorphim, return false
	cilk_sync;
	return ret;
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


