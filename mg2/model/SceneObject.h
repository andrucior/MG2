#pragma once
#include <string>
#include <set>
#include <cmath>

#include "Matrix4x4.h"
#define M_PI 3.14159265358979323846

class SceneObject {
protected:
    std::string name;
    static inline std::set<std::string> takenNames;
    static std::string makeUnique(const std::string& base);

public:
    virtual Matrix4x4 modelMatrix() const = 0;
    virtual Matrix4x4 modelMatrixInverse() const = 0;
    virtual std::string toString() = 0;
    virtual std::string toHashCode() = 0;
    virtual void transformAroundPoint(float ax, float ay, float az,
        float sx, float sy, float sz,
        const Vector3& pivot) = 0;
    void initName(const std::string& base);
    virtual void setName(const std::string& newName);

    bool isSelected = false;
    bool isHighlightedUI = false;
    bool isHighlightedViewport = false;
    bool isHighlighted = false;
    
    std::string getName() {
        return name;
    }

    float wrapAngle(float angle) {
        return fmod(fmod(angle, 2 * M_PI) + 2 * M_PI, 2 * M_PI);
    }
    virtual ~SceneObject() {
        takenNames.erase(name);
    }
};