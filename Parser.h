//
// Created by Yakir Lugasy on 21/09/2020.
//

#ifndef GCALC_PARSER_H
#define GCALC_PARSER_H

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <cctype>
#include <vector>
#include <set>

class Parser {
private:
    std::pair<std::string,std::string> io;
    std::ifstream Input = std::ifstream();
    std::ofstream Output = std::ofstream();
public:
    std::string Text;
    std::vector<std::string> Segments;
    const std::string SPACE = " ";

    explicit Parser(const std::string input = "cin", const std::string output = "cout");
    void getInput(std::string message = "");
    friend void operator<<( Parser& parser, const std::string& message);
    void disassemblyTextToSegments(bool consider_space = true, std::set<char> separators = {}, char minus_behavior = 0);
    std::pair<int,int> searchSegmentsForInnerRange(std::string left, std::string right);
    std::pair<int,int> searchSegmentsForRange(std::string left, std::string right);
    std::vector<std::string> extractFromSegments(int start_index, int end_index);
    std::string createStringFromSegments(int start_index, int end_index);
    // Defining Exceptions
    class InputError : public std::exception{};
    class OutputError : public std::exception{};
};



// Functions that manipulates strings
namespace manipulation{
    bool areEmptyString(const std::string& str);
    bool areAlphaNumeric(const std::string& str, const std::string& exceptions = "");
    bool areSpecialCharsOnly(const std::string& str);
    bool areSpecificCharsOnly(const std::string& str, const std::string& specific);
    bool areParenthesesBalanced(const std::string& str,const char& left, const char& right);
    bool areAllParenthesesBalanced(const std::string& str);
    bool arePairOfParentheses(const char& left,const char& right);
    bool areSemicolonLegal(const std::string& str);
    std::string strToLower( std::string str);
}

#endif //GCALC_PARSER_H
