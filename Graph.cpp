//
// Created by Yakir Lugasy on 21/09/2020.
//

#include "Graph.h"
#include <algorithm>


using namespace std;

bool Graph::checkVertexName(const std::string &name){  // returns true if name is valid otherwise false.
// check for allowed characters(alpha-beta or numeric only) or an empty string.
    const string allowed_chars = "[];" ;
    if(!manipulation::areAlphaNumeric(name, allowed_chars)) {
        return false;
    }
// check if name contains '[' \ ']'
    if(name.find('[') != string::npos){
        //if yes, we need to check if Parentheses are balanced.
        if(!manipulation::areParenthesesBalanced(name, '[', ']')){
            return false;
        }
    }
// check if name contains ';' (semicolon)
    if(name.find(';') != string::npos){
        // ';' not found, then name is valid.
        return true;
    }
    else{
        // ';' found, need to check if usage is legal.
        return manipulation::areSemicolonLegal(name);
    }
}


void Graph::insertVertex(const string &vertex) {

    if(!checkVertexName(vertex)){
        throw InvalidVertex(vertex);
    }

    // check if vertex already exist.
    auto search = Vertices.find(vertex);
    if (search != Vertices.end()){
        throw VertexAlreadyExist(vertex);
    }
    Vertices.insert(vertex);
}


void Graph::insertEdge(const string &src, const string& dst) {

    // check if source vertex *and* destination are exists.
    auto search_src = Vertices.find(src);
    auto search_dst = Vertices.find(dst);
    if (search_src == Vertices.end() || search_dst == Vertices.end()){
        // if we got here, then src vertex or dst vertex are not exists.
        string edge = makeEdgeStringFormat(src,dst);
        throw InvalidEdge(edge);
    }
    // check for a loop edge <x,x> (NOT ALLOWED!)
    if (src == dst){
        string edge = makeEdgeStringFormat(src,dst);
        throw IllegalLoop(edge);
    }

    // now we can make a pair.
    pair<string, string> new_edge;
    new_edge = make_pair(src, dst);

    // check if edge already exist.
    auto search = Edges.find(new_edge);
    if (search != Edges.end()){
        string edge = makeEdgeStringFormat(src,dst);
        throw EdgeAlreadyExist(edge);
    }

    Edges.insert(new_edge);

}


std::string Graph::makeEdgeStringFormat(const std::string &src, const std::string &dst) {
    string edge = "<" + src + "," + dst + ">";
    return edge;
}


std::string Graph::multiplicationFormat(const std::string &src, const std::string &dst) {
    string left = "[", right = "]", semicolon = ";";
    string format;
    format += left;
    format += src;
    format += semicolon;
    format += dst;
    format += right;
    return format;
}


Graph &Graph::operator=(const Graph &other) {
    if (this == &other) {
        return *this;
    }
    Edges = other.Edges;
    Vertices = other.Vertices;
    return *this;
}


std::ostream &operator<<(std::ostream &os, const Graph &graph) {
    string graph_str;
    for(const string& vertex : graph.Vertices){
        graph_str += vertex;
        graph_str += '\n';
    }
    graph_str += "$\n";
    for(const pair<string,string>& edge : graph.Edges){
        graph_str += edge.first;
        graph_str += ' ';
        graph_str += edge.second;
        graph_str += '\n';
    }
    return os << graph_str;
}



Graph operator+(const Graph &graph1, const Graph &graph2) {
    Graph result;
    set_union(graph1.Vertices.begin(), graph1.Vertices.end(),
              graph2.Vertices.begin(), graph2.Vertices.end() ,
              inserter(result.Vertices, result.Vertices.begin()));

    set_union(graph1.Edges.begin(), graph1.Edges.end(),
              graph2.Edges.begin(), graph2.Edges.end() ,
              inserter(result.Edges, result.Edges.begin()));
    return result;
}


Graph operator^(const Graph &graph1, const Graph &graph2) {
    Graph result;
    set_intersection(graph1.Vertices.begin(), graph1.Vertices.end(),
                     graph2.Vertices.begin(), graph2.Vertices.end() ,
                     inserter(result.Vertices, result.Vertices.begin()));

    set_intersection(graph1.Edges.begin(), graph1.Edges.end(),
                     graph2.Edges.begin(), graph2.Edges.end() ,
                     inserter(result.Edges, result.Edges.begin()));
    return result;
}


Graph operator-(const Graph &graph1, const Graph &graph2) { // Notice that: result = graph1 - graph2
    // is the same as: result = graph1 - (graph1^graph2)
    Graph result;
    // Vertices Difference
    set_difference(graph1.Vertices.begin(), graph1.Vertices.end(),
                   graph2.Vertices.begin(), graph2.Vertices.end() ,
                   inserter(result.Vertices, result.Vertices.begin()));

    // Add to result, only edges that their vertices in result.
    for( const pair<string,string>& edge : graph1.Edges ){
        string src = edge.first;
        string dst = edge.second;
        if (binary_search(result.Vertices.begin(), result.Vertices.end(), src) &&
            binary_search(result.Vertices.begin(), result.Vertices.end(), dst)){
            result.Edges.insert(edge);
        }
    }
    return result;
}


Graph operator*(const Graph &graph1, const Graph &graph2) {
    Graph result;
    string vertex;
    for (const string& vertex1 : graph1.Vertices){
        for (const string& vertex2 : graph2.Vertices){
            vertex = Graph::multiplicationFormat(vertex1,vertex2);
            result.Vertices.insert(vertex);

        }
    }
    string src, dst;
    pair<string,string> edge;
    for (const pair<string,string>& edge1 : graph1.Edges){
        for(const pair<string,string>& edge2 : graph2.Edges){
            src = Graph::multiplicationFormat(edge1.first, edge2.first);
            dst = Graph::multiplicationFormat(edge1.second, edge2.second);
            edge = make_pair(src,dst);
            result.Edges.insert(edge);
        }
    }
    return result;
}


Graph Graph::operator!() const {
    Graph result;
    result.Vertices = this->Vertices;
    pair<string,string> edge;
    for ( const string& vertex1 : this->Vertices ){
        for ( const string& vertex2 : this->Vertices ){
            if ( vertex1 == vertex2 ) continue;
            edge = make_pair(vertex1,vertex2);
            if (!binary_search(this->Edges.begin(), this->Edges.end(), edge)){
                result.Edges.insert(edge);
            }
        }
    }
    return result;
}

std::string Graph::getStringFromGraph() {
    string graph_str;
    for(const string& vertex : this->Vertices){
        graph_str += vertex;
        graph_str += '\n';
    }
    graph_str += "$\n";
    for(const pair<string,string>& edge : this->Edges){
        graph_str += edge.first;
        graph_str += ' ';
        graph_str += edge.second;
        graph_str += '\n';
    }
    return graph_str;
}


