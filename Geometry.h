//
// Created by Isabelle Hallman on 2018-09-17.
//

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/normal.hpp"
#include "glm/glm.hpp"

struct Vertex;
struct Direction;
struct ColorDbl;
struct Triangle;
struct Ray;


struct Vertex {
    Vertex(float x, float y, float z, float wIn)
            : w(wIn) {
        position = glm::vec3(x, y, z);
    }

    float w;
    glm::vec3 position;
};

struct Direction {
    Direction(float x, float y, float z) {
        vector = glm::vec3(x, y, z);
    }

    Direction(glm::vec3 inVector) {
        vector = inVector;
    }

    glm::vec3 vector;
};

struct ColorDbl {
    ColorDbl(double rIn, double gIn, double bIn) : r(rIn), g(gIn), b(bIn) { }
    float r, g, b;
};


struct Triangle {
    Triangle(Vertex &v1In, Vertex &v2In, Vertex &v3In, ColorDbl &colorIn)
            : v1(v1In), v2(v2In), v3(v3In), color(colorIn), normal(Direction(.0, .0, .0)) {
        computeNormal();
    }

    // TODO: Implement Moller Trumbore!
    bool rayIntersection(Ray &arg) {
        return true;
    }

    Vertex v1, v2, v3;
    ColorDbl color;
    Direction normal;

    void computeNormal() {
        glm::vec3 normalVec = glm::normalize(glm::cross(v1.position - v2.position, v1.position - v3.position));
        normal = Direction(normalVec);
    }
};

struct Ray {
    Ray(Vertex* startIn, Vertex* endIn, ColorDbl* colorIn)
            : startPoint(startIn), endPoint(endIn), color(colorIn), endPointTriangle(nullptr) { }

    Vertex* startPoint, * endPoint;
    ColorDbl* color;
    Triangle* endPointTriangle;
};

