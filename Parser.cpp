//
// Created by Yakir Lugasy on 21/09/2020.
//

#include "Parser.h"


Parser::Parser(const std::string input, const std::string output) {
    io.first = input;
    io.second = output;
    if(input != "cin"){
        Input = std::ifstream(input);
        if(!Input){
            throw Parser::InputError();
        }
    }
    if(output != "cout"){
        Output = std::ofstream(output);
        if(!Output){
            throw Parser::OutputError();
        }
    }
}

void Parser::getInput(std::string message) {

    if(io.first == "cin"){
        while (true) {
            if(message != "") std::cout << message;
            getline(std::cin,this->Text);
            if(this->Text.empty()) continue;
            else return;
        }
    }
    else{
        // Reading from file - no need to print message, but we need to look for EOF
        while(getline(this->Input,this->Text)){
            if(this->Text.empty()) continue;
            else return;
        }
    }
}

void operator<<(Parser& parser, const std::string &message) {
    if(parser.io.second == "cout"){
        std::cout << message;
    }
    else{
        parser.Output << message;
    }
}

void Parser::disassemblyTextToSegments(bool consider_space, std::set<char> separators, char minus_behavior) {
    Segments.clear();
    std::string word;
    for(auto letter : Text){
        if((consider_space && letter == SPACE.front())){
            if(!word.empty()){
                Segments.push_back(word);
                word.clear();
            }
            continue;
        }

        if(!separators.empty() && separators.find(letter) != separators.end()){
            if(!word.empty()){
                Segments.push_back(word);
                word.clear();
            }
            if((minus_behavior != 0) && !Segments.empty() &&
              Segments.back().front() == minus_behavior && letter == minus_behavior){
                Segments.pop_back();
                continue;
            }
            Segments.push_back(std::string(1,letter));
            continue;
        }

        word.push_back(letter);
    }

    if(!word.empty()) Segments.push_back(word);
}

/// searchSegmentsForInnerRange - search for the most inner range of segments contains left&right as edges.
///                               i.e if Text is "(1+(3+5))" so Segments is { "(","1","+","(","3","+","5",")",")" }
///                                                             with indexes   0   1   2   3   4   5   6   7   8 .
///                               so if we choose left = "(", right = ")", this function will return pair of (3,7)
///                               so if we choose left = "+", right = "3", this function will return pair of (2,4)
///                               so if we choose left = "(", right = "(", this function will return pair of (3,-1)
///                               so if we choose left = "*", right = ")", this function will return pair of (-1,-1)
/// @param left - inner left edge to search for
/// @param right - right edge to search for after left edge had found.
std::pair<int,int> Parser::searchSegmentsForInnerRange(std::string left, std::string right) {
    std::pair<int, int> result = {-1, -1};
    int segments_index = 0, left_index = -1, right_index = -1;
    for (const auto& segment : Segments) {
        if (segment == left) {
            left_index = segments_index;
        }
        segments_index++;
    }
    if (left_index == -1) {
        return result;   // returns pair of (-1,-1) if left has not found.
    }
    segments_index = left_index;
    for (auto segment = Segments.begin() + left_index; segment != Segments.end(); segment++) {
        if (*segment == right) {
            right_index = segments_index;
            break;
        }
        segments_index++;
    }
    result.first = left_index;
    result.second = right_index;
    return result;
}

std::vector<std::string> Parser::extractFromSegments(int start_index, int end_index) {
    assert(end_index >= start_index);
    assert(end_index < Segments.size());
    return std::vector<std::string>(Segments.begin()+start_index,Segments.begin()+end_index);
}

std::pair<int, int> Parser::searchSegmentsForRange(std::string left, std::string right) {
    std::pair<int, int> result = {-1, -1};
    int segments_index = 0, left_index = -1, right_index = -1;
    for (const auto& segment : Segments) {
        if (segment == left) {
            left_index = segments_index;
            break;
        }
        segments_index++;
    }
    if (left_index == -1) {
        return result;   // returns pair of (-1,-1) if left has not found.
    }
    segments_index = left_index;
    for (auto segment = Segments.begin() + left_index; segment != Segments.end(); segment++) {
        if (*segment == right) {
            right_index = segments_index;
            break;
        }
        segments_index++;
    }
    result.first = left_index;
    result.second = right_index;
    return result;
}

std::string Parser::createStringFromSegments(int start_index, int end_index) {
    assert(start_index >= 0 && start_index <= Segments.size()-1);
    assert(end_index >= start_index && end_index <= Segments.size()-1);
    std::string result;
    for(auto seg = Segments.begin()+start_index; seg <= Segments.begin()+end_index ; seg++){
        result = result + *seg;
    }
    return result;
}


/// areEmptyString - returns true if given string is empty, otherwise false.
/// @param str - the given string.
bool manipulation::areEmptyString(const std::string& str) {
    return str.empty();
}

/// areAlphaNumeric - returns true if given string are: not empty and contains only alpha-beta and numeric chars
///                       otherwise returns false.
/// @param str - the given string.
/// @param except - string contains some more chars to except. empty by default.
bool manipulation::areAlphaNumeric(const std::string& str, const std::string& except) {

    // check for an empty string ( not allowed )
    if(manipulation::areEmptyString(str)) return false;

    // checking for allowed characters
    for(const auto letter : str){
        if(isalpha(static_cast<unsigned char>(letter)) ||   // alpha-beta allowed
           isalnum(static_cast<unsigned char>(letter)) ||   // numeric allowed
           (std::string::npos != except.find(letter)) ) {   // exception allowed
            continue;
        }
        else return false;
    }
    return true;
}

/// areParenthesesBalanced - return true if (given) Parentheses are Balanced in given string.
///                          i.e if left='[', right=']' so "[x1[x3]]" -> true but "[x1]]x2[" -> false.
/// @param str - the given string.
/// @param left - the left parenthesis char.
/// @param right - the right parenthesis char.
bool manipulation::areParenthesesBalanced(const std::string &str, const char &left, const char &right) {
    std::stack<char> s;
    for(const auto letter : str){
        if( letter == left)
        {
            s.push(letter);
        }
        else if( letter == right)
        {
            if(s.empty() || !(s.top() == left && letter == right))

                return false;
            else
                s.pop();
        }
    }
    // is S empty, it return true cause parentheses are balanced, otherwise false.
    return s.empty();
}

/// areAllParenthesesBalanced - return true if all Parentheses are Balanced in given string.
///                             this function checks for: "( )","{ }","[ ]","< >".
///                             i.e (x1(z1{<y2,y3>})) -> true, (x2(<{x1<>})y2) -> false.
/// @param str - the given string.
bool manipulation::areAllParenthesesBalanced(const std::string &str) {
    std::stack<char>  S;
    for(const auto letter : str)
    {
        if(letter == '(' || letter == '{' || letter == '[' || letter =='<')
            S.push(letter);
        else if(letter == ')' || letter == '}' || letter == ']' || letter =='>')
        {
            if(S.empty() || !manipulation::arePairOfParentheses(S.top(),letter))
                return false;
            else
                S.pop();
        }
    }
    // is S empty, it return true cause parentheses are balanced, otherwise false.
    return S.empty();
}

/// arePairOfParentheses - returns true if given left & right char are a pair of correct Parentheses.
/// @param left - the left parenthesis char.
/// @param right - the right parenthesis char.
bool manipulation::arePairOfParentheses(const char& left, const char& right) {
    return ((left == '(' && right == ')') || (left == '{' && right == '}') ||
           (left == '[' && right == ']') || (left == '<' && right == '>'));
}


/// areSemicolonLegal - returns true if ";" is between "[ ]" in given str.
///                     i.e "[x1[;;;;;]" -> true, "x1[];y" -> false.
/// @param str - the given string.
bool manipulation::areSemicolonLegal(const std::string &str) {
    bool semicolon = false;
    int left_cnt = 0;
    int right_cnt = 0 ;

    for(auto const letter : str){
        if( letter == '[')
        {
            if(semicolon) return false;
            left_cnt++;
        }
        else if( letter == ']')
        {
            right_cnt++;
            semicolon = false;
        }
        else if (letter == ';')
        {
            if( right_cnt >= left_cnt ) return false;
            semicolon = true;
        }
    }
    return true;
}

/// sareSpecificCharsOnly - returns true if given string contains only one or more chars from specific string.
///                    i.e if specific="abc" and str="bcaabccba" -> true or if "str=abcabcabcd" -> false.
/// @param str - the given string.
/// @param specific - string contains specific chars to look for.
bool manipulation::areSpecificCharsOnly(const std::string &str, const std::string& specific) {
    return str.find_first_not_of(specific) == std::string::npos;
}

/// areSpecialCharsOnly - returns true if given string contains only special char like: spaces, tabs, newlines, etc..
/// @param str - the given string.
bool manipulation::areSpecialCharsOnly(const std::string& str) {
    return manipulation::areSpecificCharsOnly(str," \t\n\v\f\r");
}

/// strToLower - return a lower case string of the given string.
///              i.e if str = "Ab12C" -> "ab12c".
/// @param str - the given string.
std::string manipulation::strToLower(std::string str) {
    char word[100];
    for(unsigned int index = 0 ; index <= str.size() ; index++){
        char c = str[index];
        if(isalpha(c)){
            word[index] = tolower(c);
        }
        else{
            word[index] = c;
        }
    }
    std::string lower_str(word);
    return lower_str;
}

