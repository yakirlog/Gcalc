//
// Created by Yakir Lugasy on 21/09/2020.
//

#ifndef GCALC_GRAPH_H
#define GCALC_GRAPH_H


#include <iostream>
#include <set>
#include <string>
#include "GraphExceptions.h"
#include "Parser.h"

class Graph {
private:
// Private variables
    std::set<std::string> Vertices;                        // set of vertices.
    std::set<std::pair<std::string,std::string>> Edges;    // set of edges (pair: <"src","dst">).

// Private methods
    static bool checkVertexName(const std::string& name );
    static std::string multiplicationFormat(const std::string& src , const std::string& dst);
    static std::string makeEdgeStringFormat(const std::string& src, const std::string& dst);

public:
// Public Methods
    explicit Graph() = default;
    ~Graph() = default;
    void insertVertex(const std::string &vertex);
    void insertEdge(const std::string &src, const std::string& dst);
    std::string getStringFromGraph();
// Operators
    Graph& operator=( const Graph &other);
    Graph operator!() const;
    friend Graph operator+(const Graph& graph1, const Graph& graph2);
    friend Graph operator^(const Graph& graph1, const Graph& graph2);
    friend Graph operator-(const Graph& graph1, const Graph& graph2);
    friend Graph operator*(const Graph& graph1, const Graph& graph2);
    friend std::ostream& operator<<(std::ostream& os, const Graph& graph);

};

// Defining Exceptions (By order)
//  Vertex:
//    class InvalidVertex;        // for Invalid Vertex Name.
//    class VertexAlreadyExist;         // for case Vertex Already Exists.
//  Edge:
//    class InvalidEdge;          // for case src or dst vertices are not exists.
//    class IllegalLoop;          // for case src = dst
//    class EdgeAlreadyExist;         // for case Edge Already Exists.


#endif //GCALC_GRAPH_H
