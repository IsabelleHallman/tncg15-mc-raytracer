//
// Created by Isabelle Hallman on 2018-09-17.
//

#include <glm/vec3.hpp>

struct Vertex {
    Vertex(float xIn, float yIn, float zIn, float wIn)
            : x(xIn), y(yIn), z(zIn), w(wIn) { }
    vec3<float> position = vec3<float>(x, y, z);
    float x, y, z, w;
};

struct Direction {
    Direction(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) { }
    float x, y, z;
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

    void rayIntersection(Ray &arg) {

    }

    Vertex v1, v2, v3;
    ColorDbl color;
    Direction normal;

    void computeNormal() {
        normal = Direction(0.0, .0, .0);
    }
};