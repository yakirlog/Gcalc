//
// Created by Yakir Lugasy on 22/09/2020.
//

#include "Auxiliaries.h"

bool searchContainer(const std::set<std::string> &container, const std::string &item) {
    auto search = container.find(item);
    return search != container.end();
}

bool searchContainer(const std::set<char> &container, const char &item) {
    auto search = container.find(item);
    return search != container.end();
}

bool searchContainer(const std::vector<std::string> &container, const std::string &item) {
    return findIt(container.begin(), container.end(), item);
}


bool searchContainer(const std::map<std::string,std::shared_ptr<Graph>> &container, const std::string &item) {
    auto search = container.find(item);
    return search != container.end();
}