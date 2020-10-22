//
// Created by Yakir Lugasy on 22/09/2020.
//

#ifndef GCALC_AUXILIARIES_H
#define GCALC_AUXILIARIES_H

#include <set>
#include <map>
#include <memory>
#include "Graph.h"


bool searchContainer(const std::set<std::string>& container, const std::string& item);
bool searchContainer(const std::set<char>& container, const char& item);
bool searchContainer(const std::vector<std::string>& container, const std::string& item);
bool searchContainer(const std::map<std::string,std::shared_ptr<Graph>>& container, const std::string& item);  //change to graph*

template<class InputIt, class T>
typename std::iterator_traits<InputIt>::difference_type
countItems(InputIt first, InputIt last, const T& value)
{
    typename std::iterator_traits<InputIt>::difference_type ret = 0;
    for (; first != last; ++first) {
        if (*first == value) {
            ret++;
        }
    }
    return ret;
}

template<class InputIt, class T>
bool findIt(InputIt first, InputIt last, const T& value)
{
    for (; first != last; ++first) {
        if (*first == value) {
            return true;
        }
    }
    return false;
}

#endif //GCALC_AUXILIARIES_H
