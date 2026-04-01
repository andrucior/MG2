#include "SceneObject.h"

std::string SceneObject::makeUnique(const std::string& base)
{
    if (!takenNames.contains(base))
        return base;

    int i = 0;
    while (takenNames.contains(base + " (" + std::to_string(i) + ")"))
        i++;

    return base + " (" + std::to_string(i) + ")";   
}

void SceneObject::initName(const std::string& base) {
    name = makeUnique(base);
    takenNames.insert(name);
}

void SceneObject::setName(const std::string& newName) {
    if (newName == name) return;
    if (newName.empty()) return;

    takenNames.erase(name);
    name = makeUnique(newName);
    takenNames.insert(name);
}
