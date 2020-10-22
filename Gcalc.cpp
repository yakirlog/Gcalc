//
// Created by Yakir Lugasy on 21/09/2020.
//

#include "Gcalc.h"

int Gcalc::temp_counter = 0;

std::string Gcalc::errorMessage(const std::string& detailes, const std::string& specific){
    std::string message = "Error: " + detailes + "'" + specific + "'\n";
    return message;
}


bool Gcalc::getSession(){
    return this->Session;
}


void Gcalc::processLine(Parser &parser) {
// we need to check for illegal input
    // first we check if the input contains special chars only (like spaces,new lines, tabs etc..)
    if(manipulation::areSpecialCharsOnly(parser.Text)){
        // if so we return (and then the user will be asked to enter a new line).
        return;
    }
    // second:
        // check for allowed chars
    if(!manipulation::areAlphaNumeric(parser.Text,"{}[]()<>,|;=+-^*! ") ||
        // or for dumb expressions like: =, -=+[][[]]
       manipulation::areSpecificCharsOnly(parser.Text, "{}[]()<>,|;=+-^*!") ||
        // or for parentheses correctness '()' '[]' '<>' '{}'
       !manipulation::areAllParenthesesBalanced(parser.Text) ){
        parser << errorMessage("Unrecognized command ",parser.Text);
        return;
    }

    // now we can disassembly the input into segments
    std::set<char> text_separators;
    for(const auto& opearation : Operations){
        text_separators.insert(opearation.front());
    }
    for(const auto& separator : Separators){
        text_separators.insert(separator.front());
    }
    parser.disassemblyTextToSegments(true , text_separators , '!');

    // now we check for graph's definitions and make temp
    while(searchContainer(parser.Segments , "{")){
        bool Success = createTempGraphFromDefinition(parser);
        if(!Success){
            // in this case, an error shown to user.
            cleanTempVariables();
            return;
        }
    }

    // now we need to find out what is the kind of user's input
    // if "=" exsits, it means line is an Operation or Graph's Define.
    // we still need to check for illegal syntax.
    if(searchContainer(parser.Segments,"=") || parser.Text.find('=') != std::string::npos ){
        // "=" must be at segments's second cell.
        if(parser.Segments.size() < 3 || parser.Segments.at(1) != "="){
            parser << errorMessage("Unrecognized command ", parser.Text);
            return;
        }
        // cant be more the one "=".
        if(countItems(parser.Segments.begin(),parser.Segments.end(), "=") != 1){
            parser << errorMessage("Unrecognized command ","more than ONE equal signs.");
            return;
        }
        // need to check if variable's name is valid (first segment in Segments).
        if(!checkVariableName(parser, parser.Segments.front())){
            return;
        }
        // if "=" exist, input cant contain a command like print, save, etc.. i.e G1 = print(G2 + G3) not allowed.
        for(auto& command : Commands){
            if(searchContainer(parser.Segments,command)){
                parser << errorMessage("Unrecognized command ",parser.Text);
                return;
            }
        }
        // now we can try to perform the operation.
        executeAssignment(parser);
        return;
    }
    else{
        // if "=" is not exist, we try to execute a command.
        executeCommand(parser);
        return;
    }
}


bool Gcalc::checkVariableName(Parser& parser, const std::string& var_name) {
    // check for un-allowed characters
    if(!manipulation::areAlphaNumeric(var_name)){
        parser << errorMessage("Variable's name contains invalid characters (only alpha-numeric allowed) ", var_name);
        return false;
    }
    // first letter must be an alpha-beta
    if(!isalpha(static_cast<unsigned char>(var_name.front()))){
        parser << errorMessage("Variable's name must begin with a letter ", var_name);
        return false;
    }
    // check if name is reserved word
    if(searchContainer(Commands, var_name)){
        parser << errorMessage("Variable's name can't be a reserved word ", var_name);
        return false;
    }
    return true;
}


void Gcalc::executeAssignment(Parser &parser) {
    // we already checked if result_name is valid.
    std::string result_name = parser.Segments.front();
    Graph result;
    try{
        // we need to delete "result_name" and "=" from segments before try to do calculations.
        parser.Segments.erase(parser.Segments.begin()); // for result_name
        parser.Segments.erase(parser.Segments.begin()); // for "="
        // now we can calculate expression.
        result = simplifyExpression(parser);
    } catch(const Gcalc::SyntaxError& e){
        parser << errorMessage(e.what(), e._name);
        cleanTempVariables();
        return;
    } catch (const Gcalc::UndefinedVariable &v) {
        parser << errorMessage(v.what(), v._name);
        cleanTempVariables();
        return;
    }
    // put the result at memory
    Variables[result_name] = std::shared_ptr<Graph>(new Graph(result));
    cleanTempVariables();
}


void Gcalc::executeCommand(Parser &parser) {
// first we check for one-word command
    if(parser.Segments.size() == 1){
        if(parser.Segments.front() == "who"){
            Gcalc::doWho(parser);
        }
        else if(parser.Segments.front() == "reset"){
            Gcalc::doReset();
        }
        else if(parser.Segments.front() == "quit"){
            Gcalc::doQuit();
        }
        else{
            parser << errorMessage("Unrecognized command ", parser.Text);
        }
        return;
    }
// second we check for more complex commands:
    else if(parser.Segments.front() == "delete"){
        Gcalc::doDelete(parser);
    }
    else if(parser.Segments.front() == "print"){
        try{
            Gcalc::doPrint(parser);
        }
        catch(const Gcalc::SyntaxError& e){
            parser << errorMessage(e.what(), e._name);
            cleanTempVariables();
            return;
        } catch (const Gcalc::UndefinedVariable &v) {
            parser << errorMessage(v.what(), v._name);
            cleanTempVariables();
            return;
        }
    }
    else if(parser.Segments.front() == "save"){
        Gcalc::doSave(parser);
    }
    else{
        parser << errorMessage("Unrecognized command ", parser.Text);
    }
}


void Gcalc::doSave(Parser& parser) {
    // Diet "()"
    std::vector<std::string> copy;
    for(auto element = parser.Segments.begin() ; element != parser.Segments.end() ; element++ ){
        if( (*(element) == "(" && *(element + 1) == "(") || (*(element) == ")" && *(element + 1) == ")") ){
            continue;
        }
        copy.push_back(*element);
    }
}


void Gcalc::doPrint(Parser &parser) {
    // first we check for allowed chars.
    if(!manipulation::areAlphaNumeric(parser.Text,"{}[]()<>,|;+-^*! ")){
        parser << errorMessage("Unrecognized command ", parser.Text);
        return;
    }
    if(parser.Segments.at(1) != "(" || parser.Segments.back() != ")"){
        parser << errorMessage("Unrecognized command ", parser.Text);
    }

    Graph to_print;
    // we need to delete "print" from segments before try to do calculations.
    parser.Segments.erase(parser.Segments.begin());
    // now we can calculate expression.
    to_print = simplifyExpression(parser);
    // finally we can print.
    parser << to_print.getStringFromGraph();
    cleanTempVariables();
}


void Gcalc::doDelete(Parser& parser) {
    // in this case only chars that is alpha-beta, numerical or '(' \ ')' are allowed.
    if(!manipulation::areAlphaNumeric(parser.Text,"()")){
        parser << errorMessage("Unrecognized command ", parser.Text);
        return;
    }
    // lets get the inner '(' ')'
    // for example if "delete ( ( (  G1 ) ) )" : left_index=3, right_index=5
    // Segments index's: 0    1 2 3  4  5 6 7
    std::pair<int,int> result = parser.searchSegmentsForInnerRange("(",")");
    int left_index = result.first, right_index = result.second;
    if(right_index - left_index != 2){
        // i.e "delete(G1 + G2)" or "delete(G 1)
        parser << errorMessage("Unrecognized command ", parser.Text);
        return;
    }
    // all that's left is to check if variable exists in memory.
    std::string var_name = parser.Segments.at(left_index+1);
    if(!searchContainer(Variables , var_name)){
        Gcalc::errorMessage("Undefined variable ",var_name);
        return;
    }
    // if we got here, command is legal, and we can delete variable.
    Variables.erase(var_name);
}

void Gcalc::doReset() {
    if(!Variables.empty()){
        Variables.clear();
    }
}

void Gcalc::doWho(Parser& parser) {
    // first we check if there are any variables at storage
    if(Variables.empty()){
        return;
    }
    // otherwise lets print names of variables
    for(const auto& var : Variables){
        parser << (var.first + "\n");
    }
}


void Gcalc::doQuit(){
    this->Session = false;
}

bool Gcalc::createTempGraphFromDefinition(Parser &parser) {
    Graph temp;
    std::pair<int, int> result = parser.searchSegmentsForRange("{", "}");
    int left_index = result.first, right_index = result.second;
    std::vector<std::string> definition(parser.Segments.begin()+left_index , parser.Segments.begin()+right_index+1);
    std::string definition_str = parser.createStringFromSegments(left_index,right_index);
    try{
        temp = defineGraph(definition , definition_str);
    }
    catch(const Gcalc::SyntaxError& e){
        parser << errorMessage(e.what(), e._name);
        return false;
    } catch (const InvalidVertex &v) {
        parser << errorMessage(v.what(), v._name);
        return false;
    } catch (const VertexAlreadyExist &v) {
        parser << errorMessage(v.what(), v._name);
        return false;
    }catch (const InvalidEdge &e) {
        parser << errorMessage(e.what(), e._name);
        return false;
    } catch (const IllegalLoop &e) {
        parser << errorMessage(e.what(), e._name);
        return false;
    } catch (const EdgeAlreadyExist &e) {
        parser << errorMessage(e.what(), e._name);
        return false;
    }
    std::string temp_name = "_Temp" + std::to_string(Gcalc::temp_counter);
    parser.Segments.at(left_index) = temp_name;
    parser.Segments.erase(parser.Segments.begin()+left_index+1 ,parser.Segments.begin()+right_index+1);
    Variables[temp_name] = std::shared_ptr<Graph>(new Graph(temp));
    Gcalc::temp_counter++;
    return true;
}


Graph Gcalc::simplifyExpression(Parser& parser) {
    Graph temp;
    std::pair<int, int> result = parser.searchSegmentsForInnerRange("(", ")");
    int left_index = result.first, right_index = result.second;

    if(left_index == -1){
        return calculateExpression(parser.Segments, parser.Text);
    }
    else if(right_index - left_index == 1){ // for case like print(G1 + ())
        throw SyntaxError(parser.Text);
    }
    else if((right_index - left_index == 2) || (right_index - left_index == 3 && parser.Segments.at(left_index+1) == "!")){
        parser.Segments.erase(parser.Segments.begin()+right_index);
        parser.Segments.erase(parser.Segments.begin()+left_index);
    }
    else{
        std::vector<std::string> expression(parser.Segments.begin()+left_index+1 , parser.Segments.begin()+right_index);
        temp = calculateExpression(expression,parser.Text);
        std::string temp_name = "_Temp" + std::to_string(Gcalc::temp_counter);
        parser.Segments.at(left_index) = temp_name;
        parser.Segments.erase(parser.Segments.begin()+left_index+1 ,parser.Segments.begin()+right_index+1);
        Variables[temp_name] = std::shared_ptr<Graph>(new Graph(temp));
        Gcalc::temp_counter++;
    }
    return simplifyExpression(parser);
}


Graph Gcalc::defineGraph(std::vector<std::string>& definition, std::string& definition_str){
    // first we check for an empty graph.
    if(definition.size() <= 3){
        // {} allowed
        if(definition.at(0) == "{" && definition.at(1) == "}"){
            return Graph();
        }
        // {|} allowed
        else if(definition.at(0) == "{" && definition.at(1) == "|" && definition.at(2) == "}"){
            return Graph();
        }
        else{
            throw SyntaxError(definition_str);
        }
    }
    // {|<x1,x2>} not allowed!
    if(definition.at(0) == "{" && definition.at(1) == "|" && definition.at(2) != "}"){
        throw SyntaxError(definition_str);
    }
    // if we got here - graph is not empty
    // check again for syntax
    if( (definition.front() != "{" || definition.back() != "}") ||    // making sure "{}" are in the right place and appears only one time
        countItems(definition.begin(),definition.end(), "{") != 1 ||
        countItems(definition.begin(),definition.end(), "}") != 1 ||
        countItems(definition.begin(),definition.end(), "|") != 1){
        throw SyntaxError(definition_str);
    }
    // let extract vertices and edges
    bool start_define = false;  // for {
    bool sep = false;           // for |
    bool vertex_flag = false;
    Graph temp;
    std::string src, dst;
    for(auto it = definition.begin()+1 ; it != definition.end() ; it++){
        // check for illegal segments next to each other
        if((*(it-1) == "{" && *it == ",") || (*(it-1) == "|" && *it == ",") || (*(it-1) == "," && *it == ",") ||
           (*(it-1) == "<" && *it == "<") || (*(it-1) == ">" && *it == ">") || (*(it-1) == ">" && *it == "<") ||
           (*(it-1) == "<" && *it == ",")){
            throw SyntaxError(definition_str);
        }
        // update flags
        if(*it == "|") sep = true;
        else if(*it == ",") vertex_flag = false;
        else if(*(it-1) == "{" ) start_define = true; // cause we start at index 1 and "{" at index 0
        else if (*it == "}") break;

        if(*(it) == "{" || *it == "|" || *it == "<" || *it == ">" || *it == ",")  continue;
        // means now we extracting vertices
        if(start_define && !sep){
            if(!vertex_flag){
                temp.insertVertex(*it);
                vertex_flag = true;
            }
            else{
                throw SyntaxError(definition_str);
            }
        }

        // means now we extracting vertices
        if(start_define && sep){
            if(*(it-1) == "<" && *(it+1) == ","){
                src = *it;
            }
            else if(*(it-1) == "," && *(it+1) == ">"){
                dst = *it;
                if(src.empty() || dst.empty()){
                    throw SyntaxError(definition_str);
                }
                else{
                    temp.insertEdge(src,dst);
                }
            }
            else{
                throw SyntaxError(definition_str);
            }
        }
    }
    return temp;
}

void Gcalc::cleanTempVariables(){
    auto var = Gcalc::Variables.begin();
    while ( var != Gcalc::Variables.end() ){
        if( var->first.front() == '_' ){
            var = Gcalc::Variables.erase(var);
        }else{
            ++var;
        }
    }
    Gcalc::temp_counter = 0;
}

Graph Gcalc::calculateExpression(std::vector<std::string> &expression, std::string &expression_str) {
    // first we need to check for Expression's syntax
    checkExpressionSyntax(expression, expression_str); // throws an exception if not valid.
    // if expression is just variable or !variable, we will return it.
    if(expression.size() == 1 || expression.size() == 2 ){
        // for commands like "!!g2" or "!!!!g2" => g2
        if(expression.size() == 1 && searchContainer(Variables, expression.back())){
            Graph temp = *Variables.at(expression.back());
            return temp;
        }
        // for commands like "!!!g2" or "!!!!!g2" => !g2
        else if(expression.size() == 2 && searchContainer(Variables, expression.back()) && expression.at(0) == "!"){
            return !*Variables.at(expression.back());
        }
        else{
            throw UndefinedVariable(expression_str);
        }
    }
    // let simplify expression
    std::vector<std::string> new_exp;
    int complement = false;
    for(auto& element : expression){
        if(element != "!"){
            if(complement){
                new_exp.push_back("!" + element);
            }
            else{
                new_exp.push_back(element);
            }
            complement = false;
        }
        else{
            complement = true;
        }
    }

    Graph graph1, graph2, result;
    std::string left_var, right_var;
    bool first_time = true;
    for(auto element = new_exp.begin() ; element != new_exp.end() ; element++) {
        char token = (*element).front();
        if (token == '+' || token == '-' || token == '^' || token == '*') {
            left_var = *(element - 1);
            right_var = *(element + 1);
            if (left_var.front() == '!') {
                graph1 = !*Variables.at(left_var.substr(1)); // "!G1" -> |substr(1)| -> "G1"
            } else{
                graph1 = *Variables.at(left_var);
            }
            if (right_var.front() == '!') {
                graph2 = !*Variables.at(right_var.substr(1)); // "!G1" -> |substr(1)| -> "G1"
            } else{
                graph2 = *Variables.at(right_var);
            }

            switch(token){
                case '+':{
                    if (first_time) result = graph1 + graph2;
                    else result = result + graph2;
                    break;
                }
                case '^':{
                    if (first_time) result = graph1 ^ graph2;
                    else result = result ^ graph2;
                    break;
                }
                case '-':{
                    if (first_time) result = graph1 - graph2;
                    else result = result - graph2;
                    break;
                }
                case '*':{
                    if (first_time) result = graph1 * graph2;
                    else result = result * graph2;
                    break;
                }
                default:{
                    break;
                }
            }
            first_time = false;
        }
    }
    return result;
}


// throw an exceptions if syntax is not valid or variables are unknown.
void Gcalc::checkExpressionSyntax(std::vector<std::string> &expression, std::string& expression_str) {
    // even indexes must be variable and odd indexes must be operation: G3 = G1 + G2 - G3
    // Unless there is a "!" or "!!!...!!" : G3 = G1 + !G2
    enum KIND {VARIABLE, OPERATOR, UNKNOWN };
    KIND current = UNKNOWN , last = UNKNOWN;
    bool skip = false;      // in case there are operator '!'
    int element_index = 0;
    for(auto element : expression ){
        char token = element.front();
        if(searchContainer(Operations, element)){
            // if we got here, token is operator
            token == '!' ? skip = true : skip = false;
            current = OPERATOR;
            if( (!skip && current == last) || (!skip&&last == UNKNOWN)){
                // first condition means that the last *2* tokens were operator! like G1+-G2 and thats Illegal (except "!").
                // second condition is for illegal command like "-G1+G2" or "*G2 - G1".
                throw SyntaxError(expression_str);
            }
            last = current;
        }
        else {
            // if we got here, token is possibly a variable
            current = VARIABLE;
            if( (!skip && current == last) || searchContainer(Operations,element)){
                // first condition means that the last *2* tokens were variables! like "G1 G2 + G3" -> Illegal.
                // second condition is just to make sure variable name is not an opearation.
                throw SyntaxError(expression_str);
            }
            // if token is variable, we need to make sure it is known to Gcalc memory.
            // this why we gonna check if segments contains "="
            if( !searchContainer(Variables,element)){
                throw UndefinedVariable(element);
            }

            last = current;
        }
        // this one is for a case when an operator shows in the last segment (invalid) i.e: G1 = G2 + G3 -
        if(element_index == expression.size() - 1 && current == OPERATOR){
            throw SyntaxError(expression_str);
        }

        element_index++;
    }
    // if we got here syntax is legal and we are gonna start calculate.
}






