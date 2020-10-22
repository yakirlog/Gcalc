//
// Created by Yakir Lugasy on 22/09/2020.
//

#ifndef GCALC_GRAPHEXCEPTIONS_H
#define GCALC_GRAPHEXCEPTIONS_H

#include <exception>
#include <string>

// Defining Exceptions (By order)
//
//  Vertex:
//    class InvalidVertex;        // for Invalid Vertex Name.
//    class VertexAlreadyExist;   // for case Vertex Already Exists.
//
//  Edge:
//    class InvalidEdge;          // for case src or dst vertices are not exists.
//    class IllegalLoop;          // for case src = dst
//    class EdgeAlreadyExist;     // for case Edge Already Exists.


class InvalidVertex : public std::exception{
public:
    std::string _name;
    explicit InvalidVertex(std::string name): _name(name){}
    const char *what() const noexcept override{
        return "Invalid Vertex ";
    }
};


class VertexAlreadyExist : public std::exception{
public:
    std::string _name;
    explicit VertexAlreadyExist(std::string name): _name(name){}
    const char *what() const noexcept override{
        return "Vertex is Already Exist ";
    }
};


class InvalidEdge : public std::exception{
public:
    std::string _name;
    explicit InvalidEdge(std::string name): _name(name){}
    const char *what() const noexcept override{
        return "Invalid Edge ";
    }
};


class IllegalLoop : public std::exception{
public:
    std::string _name;
    explicit IllegalLoop(std::string name): _name(name){}
    const char *what() const noexcept override{
        return "Loops are not allowed! ";
    }
};


class EdgeAlreadyExist : public std::exception{
public:
    std::string _name;
    explicit EdgeAlreadyExist(std::string name): _name(name){}
    const char *what() const noexcept override{
        return "Edge is Already Exist ";
    }
};


#endif //GCALC_GRAPHEXCEPTIONS_H
