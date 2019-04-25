# subgraph_isomorphism

The code is tested using Cilk+ along with Clang 3.9.0 
In order to compile parallel ullmann using Cilk, it requires Clang installation in order to use two features: cilk_spawn and cilk_sync. 

To compile the code, use the following command: clang++ -fcilkplus -std=c++11 pullmann.cpp -o pullmann