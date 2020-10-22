#include <iostream>
#include "Parser.h"
#include "Gcalc.h"

using namespace std;

int main(int argc, char **argsv){

    Gcalc gcalc;
    switch(argc){

        case 1:{
            // means we are reading and writing from/to standard channels.
            try{
                Parser parser("cin","cout");
                while(gcalc.getSession()) {
                    parser.getInput(gcalc.Prompt);
                    gcalc.processLine(parser);
                }
            } catch (const std::exception& e) {
                std::cerr << "Fatal Error: " << e.what() << "\n";
                gcalc.doQuit();
            }
            break;
        }

        case 3: {
            // means we are reading and writing from/to files.
            try{
                Parser parser(argsv[1],argsv[2]);
                while(gcalc.getSession()) {
                    parser.getInput();
                    gcalc.processLine(parser);
                }
            } catch (const Parser::InputError& e) {
                std::cerr << "Fatal Error: Cant open input file.\n";
                return 1;
            } catch (const Parser::OutputError& e) {
                std::cerr << "Fatal Error: Cant open output file.\n";
                return 1;
            } catch (const std::exception& e) {
                std::cerr << "Fatal Error: " << e.what() << "\n";
                gcalc.doQuit();
            }
            break;
        }

        default: {
            std::cerr << "Fatal Error: Cant open output file.\n";
            return 1;
        }
    }
    return 0;
}
