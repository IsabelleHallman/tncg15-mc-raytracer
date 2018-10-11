//
// Created by Isabelle Hallman on 2018-09-17.
//

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/normal.hpp"
#include "glm/glm.hpp"

const int LAMBERTIAN = 0;
const int OREN_NAYAR = 1;
const int PERFECT_REFLECTOR = 2;


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
        vector = glm::normalize(glm::vec3(x, y, z));
    }

    Direction(glm::vec3 inVector) {
        vector = glm::normalize(inVector);
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
    Ray(Vertex *startIn, Direction directionIn)
            : startPoint(startIn), endPoint(Vertex()), direction(directionIn), color(nullptr),
              endPointTriangle(nullptr), tangentSpace(glm::vec3(0.0)), normal(Direction()), hasIntersected(false) { }

    Ray() : startPoint(nullptr), endPoint(Vertex()), color(nullptr), endPointTriangle(nullptr),
            tangentSpace(glm::vec3(0.0)), normal(Direction()), direction(Direction()), hasIntersected(false) { }

    ~Ray() {
        delete startPoint;
        delete color;
        delete endPointTriangle;
    }

    glm::vec3 tangentSpace;
    Vertex* startPoint;
    Vertex endPoint;
    Direction normal;
    Direction direction;
    ColorDbl* color;
    Triangle* endPointTriangle;
    bool hasIntersected;
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
            if (!ray.hasIntersected || ray.tangentSpace.x > t) {
                ray.endPointTriangle = this;
                ray.endPoint = Vertex(u * edge1 + v * edge2);
                ray.normal = normal;
                ray.color = &(this->color);
                ray.tangentSpace = glm::vec3(t, u, v);
                ray.hasIntersected = true;
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

    ~MeshObject(){
        delete [] triangles;
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

struct ImplicitSphere {

public:
    ImplicitSphere(float radiusIn, Vertex centerPos, ColorDbl colorIn)
            : radius(radiusIn), center(centerPos), color(colorIn), radiusSquared(glm::pow(radiusIn, 2)) { }

    bool rayIntersection(Ray &ray) {
        float a = 1; // Dot product of rays direction with itself
        glm::vec3 dirRayOriginToCenter = ray.startPoint->position - center.position;
        float b = glm::dot((2.f * ray.direction.vector),dirRayOriginToCenter);
        float c = glm::dot(dirRayOriginToCenter, dirRayOriginToCenter) - radiusSquared;

        float discriminant = glm::pow(b/2, 2) - a*c;
        if (discriminant < 0.f)
            return false;

        float d = -b/2 + glm::sqrt(discriminant);
        if (d < 0.f)
            d = -b/2 - glm::sqrt(discriminant);

        if (d < 0.f)
            return false;

        ray.color = &color;
        ray.tangentSpace = glm::vec3(d, 1, 1);
        ray.hasIntersected = true;
        ray.endPoint = Vertex(ray.startPoint->position + d * ray.direction.vector);
        ray.normal = Direction(ray.endPoint.position - center.position);

        return true;
    }

    bool evaluate(glm::vec3 position) {
        return (glm::pow(glm::length(position - center.position), 2) == radiusSquared);
    }

private:
    float radius;
    float radiusSquared;
    Vertex center;
    ColorDbl color;
};

struct Light {
    Light(Triangle& areaLightIn, ColorDbl colorIn)
            : areaLight(areaLightIn), color(colorIn) { }

    ColorDbl color;
    Triangle areaLight;
};

struct Material {

    Material(ColorDbl colorIn, float alphaIn, float specularIn, int typeIn)
            : color(colorIn), alpha(alphaIn), specular(specularIn), type(typeIn) {}

    float getBRDF(Vertex x, Direction wIn, Direction wOut){
        return 0;
    }

    ColorDbl color;
    float alpha;
    float specular;
    int type;
};