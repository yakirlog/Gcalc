//
// Created by Yakir Lugasy on 21/09/2020.
//

#ifndef GCALC_GCALC_H
#define GCALC_GCALC_H

#include <memory>
#include <string>
#include <map>
#include "Parser.h"
#include "Graph.h"
#include "Auxiliaries.h"

class Gcalc {
private:
    bool Session = true;
    std::map<std::string,std::shared_ptr<Graph>> Variables;
    const std::set<std::string> Commands = {"print", "delete", "load", "save", "reset", "who", "quit"};
    const std::set<std::string> Operations = {"=","+","-","^","*","!"};
    const std::set<std::string> Separators = {"{","}","(",")","<",">",",","|"};
public:
    const std::string Prompt = "Gcalc> ";
    static int temp_counter;
    bool getSession();
    static std::string errorMessage(const std::string& detailes, const std::string& specific);
    bool checkVariableName (Parser& parser, const std::string& var_name);
    Graph defineGraph(std::vector<std::string>& definition, std::string& definition_str);
    bool createTempGraphFromDefinition(Parser& parser);
    Graph simplifyExpression(Parser& parser);
    void processLine(Parser& parser);
    void executeAssignment(Parser& parser);
    void executeCommand(Parser& parser);

    Graph calculateExpression(std::vector<std::string>& expression, std::string& expression_str);
    void checkExpressionSyntax(std::vector<std::string>& expression, std::string& expression_str);
    void cleanTempVariables();



    void doPrint(Parser& parser);
    void doDelete(Parser& parser);
    void doLoad(); // todo
    void doSave(Parser& parser); // todo
    void doReset();
    void doWho(Parser& parser);
    void doQuit();

    // Defining Exceptions

    class SyntaxError : public std::exception{
    public:
        std::string _name;
        explicit SyntaxError(std::string name): _name(name){}
        const char *what() const noexcept override{
            return "Syntax Error ";
        }
    };

    class UndefinedVariable  : public std::exception{
    public:
        std::string _name;
        explicit UndefinedVariable(std::string name): _name(name){}
        const char *what() const noexcept override{
            return "Undefined Variable  ";
        }
    };


};


#endif //GCALC_GCALC_H
