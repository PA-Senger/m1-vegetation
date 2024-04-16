#include "json_helpers.hpp"
#include "json.hpp"
#include "tree.hpp"
#include <fstream>

std::vector<Tree> createLibraryFromJson(nlohmann::json const &jsonData) {
    std::vector<Tree> treeLibrary;
    for (auto &element : jsonData["elements"]) {
        treeLibrary.push_back(createTreeFromJson(element));
    }

    return treeLibrary;
}