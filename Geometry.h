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
    Vertex() : position(glm::vec3(.0, .0, .0)), w(1.0){ }

    Vertex(float x, float y, float z, float wIn)
            : position(glm::vec3(x, y, z)), w(wIn) { }

    Vertex(glm::vec3 positionIn) : w(1.0), position(positionIn) {}

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

    Direction() {
        vector = glm::vec3(.0, .0, .0);
    }

    glm::vec3 vector;
};

struct ColorDbl {
    ColorDbl() : r(.0), g(.0), b(.0) {}
    ColorDbl(double rIn, double gIn, double bIn) : r(rIn), g(gIn), b(bIn) { }
    double r, g, b;
};

struct Ray {
    Ray(Vertex* startIn, Vertex* endIn)
            : startPoint(startIn), endPoint(endIn), color(nullptr), endPointTriangle(nullptr), tangentSpace(glm::vec3(0.0)) {
        direction = Direction(startPoint->position - endPoint->position);
    }

    Ray() : startPoint(nullptr), endPoint(nullptr), color(nullptr), endPointTriangle(nullptr), tangentSpace(glm::vec3(0.0)) {
        direction = Direction();
    }

    glm::vec3 tangentSpace;
    Vertex* startPoint, * endPoint;
    Direction direction;
    ColorDbl* color;
    Triangle* endPointTriangle;
};

struct Triangle {
    Triangle() : v1(Vertex()), v2(Vertex()), v3(Vertex()), color(ColorDbl()), normal(Direction()) {}

    Triangle(Vertex &v1In, Vertex &v2In, Vertex &v3In, ColorDbl &colorIn)
            : v1(v1In), v2(v2In), v3(v3In), color(colorIn), normal(Direction(.0, .0, .0)) {
        computeNormal();
    }

    bool rayIntersection(Ray &ray) {
        // u v describes points on Moller Trumbore, t where on the ray we are
        const float EPSILON = 0.0000001;

        glm::vec3 T = ray.startPoint->position - v1.position;
        glm::vec3 edge1 = v2.position - v1.position;
        glm::vec3 edge2 = v3.position - v1.position;
        glm::vec3 direction = ray.direction.vector;
        glm::vec3 P = glm::cross(direction, edge2);
        glm::vec3 Q = glm::cross(T, edge1);

        // Avoid division by 0
        float a = glm::dot(P, edge1);
        if (a < EPSILON && a > -EPSILON) return false;

        float f = 1 / a;

        float u = glm::dot(P, T) * f;
        float v = glm::dot(Q, direction) * f;

        if (u + v > 1 || u < 0 || v < 0) return false;

        float t = glm::dot(Q, edge2) * f;

        if (t > EPSILON) {
            if (ray.endPointTriangle == nullptr || ray.tangentSpace.x > t) {
                ray.endPointTriangle = this;
                ray.color = &(this->color);
                ray.tangentSpace = glm::vec3(t, u, v);
                return true;
            }
        }
        return false;
    }

    Vertex v1, v2, v3;
    ColorDbl color;
    Direction normal;

    void computeNormal() {
        glm::vec3 normalVec = glm::normalize(glm::cross(v1.position - v2.position, v3.position - v2.position));
        normal = Direction(normalVec);
    }
};

struct MeshObject {
    MeshObject() : color(ColorDbl()), numTriangles(4) {
        triangles = new Triangle[numTriangles];
    }

    MeshObject(Vertex position, ColorDbl color, int numTrianglesIn) : numTriangles(numTrianglesIn) {
        triangles = new Triangle[numTriangles];
    }

    bool rayIntersection(Ray &ray) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].rayIntersection(ray);
        }
        return ray.endPointTriangle != nullptr;
    }

    int numTriangles;
    ColorDbl color;
    Triangle* triangles;
};

struct Tetrahedron : MeshObject {
    Tetrahedron() : Tetrahedron(Vertex(glm::vec3(8.0, 3.0, -4.0)), ColorDbl(0.0, 0.0, 0.0)) { }

    Tetrahedron(Vertex position, ColorDbl color) : MeshObject(position, color, 4) {
        Vertex v0 = Vertex(glm::vec3(1.0, 1.0, 1.0) + position.position);
        Vertex v1 = Vertex(glm::vec3(1.0, -1.0f, -1.0f) + position.position);
        Vertex v2 = Vertex(glm::vec3(-1.0f, 1.0, -1.0f) + position.position);
        Vertex v3 = Vertex(glm::vec3(-1.0f, -1.0f, 1.0) + position.position);

        // TODO: Arbitrarily chosen order of vertices - normal might be wrong.
        triangles[0] = Triangle(v0, v1, v2, color);
        triangles[1] = Triangle(v0, v1, v3, color);
        triangles[2] = Triangle(v0, v2, v3, color);
        triangles[3] = Triangle(v1, v2, v3, color);
    }
};

struct ImplicitObject {
    ImplicitObject() { }

    bool rayIntersection(Ray &ray) {
        std::cout << "Raying in parent class" << std::endl;
        return true;
    }

    virtual bool evaluate() {
        std::cout << "Evaluating in parent class" << std::endl;
        return true;
    }
};

struct ImplicitCircle : ImplicitObject {
    ImplicitCircle() : ImplicitObject() { }

    // Override
    bool evaluate() {
        std::cout << "Evaluating in child class" << std::endl;
        return true;
    }
};