//
// Created by Isabelle Hallman on 2018-09-17.
//

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/normal.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/vector_angle.hpp"

struct Vertex;
struct Direction;
struct ColorDbl;
struct Material;
struct Intersection;
struct Ray;
struct Triangle;
struct MeshObject;
struct Tetrahedron;
struct ImplicitSphere;
struct Light;

const float EPSILON = 1e-4;

const int LAMBERTIAN = 0;
const int OREN_NAYAR = 1;
const int PERFECT_REFLECTOR = 2;
const int LIGHT = 3;


struct Vertex {
    Vertex() : position(glm::vec3(.0, .0, .0)), w(1.0){ }

    Vertex(float x, float y, float z, float wIn)
            : position(glm::vec3(x, y, z)), w(wIn) { }

    Vertex(glm::vec3 positionIn) : position(positionIn), w(1.0) {}

    void print() {
        std::cout << "[" << position.x << ", " << position.y << ", " << position.z << "]" << std::endl;
    }

    bool operator==(const Vertex& b) const {
        return (glm::abs(position.x - b.position.x) < EPSILON
                && glm::abs(position.y - b.position.y) < EPSILON
                && glm::abs(position.z - b.position.z) < EPSILON);
    }

    bool operator!=(const Vertex& b) const {
        return !(*this == b);
    }

    glm::vec3 position;
    float w;
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

    bool operator==(const Direction& other) const {
        return glm::abs(other.vector.x - vector.x) < EPSILON
               && glm::abs(other.vector.y - vector.y) < EPSILON
               && glm::abs(other.vector.z - vector.z) < EPSILON;
    }

    glm::vec3 vector;
};

struct ColorDbl {

    ColorDbl() : r(.0), g(.0), b(.0) { }

    ColorDbl(double rIn, double gIn, double bIn) : r(rIn), g(gIn), b(bIn) { }

    ColorDbl(glm::vec3 color) : r(color.r), g(color.g), b(color.b) {}

    double r, g, b;

    ColorDbl& operator+=(const ColorDbl& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    ColorDbl& operator/=(const float denominator) {
        r /= denominator;
        g /= denominator;
        b /= denominator;
        return *this;
    }

    ColorDbl operator/(const float denominator) {
        return ColorDbl(r/denominator, g/denominator, b/denominator);
    }

    bool operator==(const ColorDbl& other) const {
        return glm::abs(other.r - r) < EPSILON
               && glm::abs(other.g - g) < EPSILON
               && glm::abs(other.b - b) < EPSILON;
    }

    ColorDbl operator*(const glm::vec3& other) const {
        return ColorDbl(r * other.r, g * other.g, b * other.b);
    }
};

struct Intersection {

    Intersection(Vertex intersection, Direction normalIn, Material *mat, float dist) {
        Intersection(intersection, normalIn, mat, dist, normalIn);
    }

    Intersection(Vertex intersection, Direction normalIn, Material *mat, float dist, Direction &rayDirection)
            : position(intersection), normal(normalIn), material(mat), distanceToRayOrigin(dist) {
        axisX = Direction(rayDirection.vector - glm::dot(rayDirection.vector, normal.vector) * normal.vector);
        axisY = Direction(glm::cross(-1.f * axisX.vector, normal.vector));
        worldToLocalMatrix = glm::inverse(glm::mat4(axisX.vector.x, axisX.vector.y, axisX.vector.z, 0.0,
                                       axisY.vector.x, axisY.vector.y, axisY.vector.z, 0.0,
                                       normal.vector.x, normal.vector.y, normal.vector.z, 0.0,
                                       0.0, 0.0, 0.0, 1.0));
    }

    Vertex position;
    Direction normal, axisX, axisY;
    Material* material;
    glm::mat4 worldToLocalMatrix;
    float distanceToRayOrigin;
};

struct Material {
    Material() : color(ColorDbl()), alpha(1.0), specular(0.0), type(LAMBERTIAN), rho(glm::vec3(0.5)) {
        rhoOverPi = glm::one_over_pi<float>() * rho;
    }

    Material(ColorDbl colorIn, float alphaIn, float specularIn, int typeIn, glm::vec3 rhoIn)
            : color(colorIn), alpha(alphaIn), specular(specularIn), type(typeIn), rho(rhoIn) {
        rhoOverPi = glm::one_over_pi<float>() * rho;
        if (type == OREN_NAYAR) {
            //TODO: remove hard-coding of roughness
            float roughness = 5.0;
            float roughnessSquared = roughness * roughness;
            orenA = 1 - (0.5 * (roughnessSquared / (roughnessSquared + 0.33)));
            orenB = 0.45 * (roughnessSquared / (roughnessSquared + 0.09));
        } else {
            orenA = orenB = 0.0;
        }
    }

    glm::vec3 getBRDF(Intersection& intersection, Direction& wIn, Direction& wOut) {
        //wIn towards lightsource, wOut towards observer
        float alpha, beta, thetaR, thetaI, phiDiff;
        glm::vec3 wInLocal, wOutLocal;

        switch (type) {
            case LAMBERTIAN:
                // TODO: Use different values of pho for R, G, B
                return rhoOverPi;
            case OREN_NAYAR:
                wInLocal = glm::vec3(intersection.worldToLocalMatrix * glm::vec4(wIn.vector.x, wIn.vector.y, wIn.vector.z, 1.0));
                wOutLocal = glm::vec3(intersection.worldToLocalMatrix * glm::vec4(wOut.vector.x, wOut.vector.y, wOut.vector.z, 1.0));

                thetaR = glm::acos(wOutLocal.z);
                thetaI = glm::acos(wInLocal.z);
                phiDiff = glm::atan(wOutLocal.y, wOutLocal.x) - glm::atan(wInLocal.y, wInLocal.x);
                alpha = glm::max(thetaR, thetaI);
                beta = glm::min(thetaR, thetaI);

                return rhoOverPi * (orenA + orenB *
                        glm::max(0.0f, glm::cos(phiDiff)) * glm::sin(alpha) * glm::tan(beta));
            case PERFECT_REFLECTOR:
                // TODO: In case
                return glm::vec3(1.0);
            case LIGHT:
                return rhoOverPi;
            default:
                return glm::vec3(0.0);
        }
    }

    ColorDbl color;
    float alpha;
    float specular;
    int type;
    glm::vec3 rho;
    glm::vec3 rhoOverPi;
    float orenA, orenB;
};

struct Ray {
    Ray(Vertex *startIn, Direction directionIn)
            : startPoint(startIn), direction(directionIn), intersection(nullptr) { }

    Ray() : startPoint(nullptr), direction(Direction()), intersection(nullptr) { }

    ~Ray() {
        if(intersection)
            delete intersection;
    }

    Vertex* startPoint;
    Direction direction;
    Intersection* intersection;
};

struct Triangle {
    Triangle() : v1(Vertex()), v2(Vertex()), v3(Vertex()),
                 material(&defaultMaterial), normal(Direction()) {
        edge1 = v2.position - v1.position;
        edge2 =  v3.position - v1.position;
        area = 0.5 * glm::length(glm::cross(edge1, edge2));
        computeNormal();
    }

    Triangle(Vertex &v1In, Vertex &v2In, Vertex &v3In, Material* materialIn)
            : v1(v1In), v2(v2In), v3(v3In), material(materialIn), normal(Direction(.0, .0, .0)) {
        edge1 = v2.position - v1.position;
        edge2 =  v3.position - v1.position;
        area = 0.5 * glm::length(glm::cross(edge1, edge2));
        computeNormal();
    }

    bool rayIntersection(Ray &ray) {
        // u v describes points on Moller Trumbore, t where on the ray we are

        glm::vec3 T = ray.startPoint->position - v1.position;
        glm::vec3 direction = ray.direction.vector;
        glm::vec3 P = glm::cross(direction, edge2);
        glm::vec3 Q = glm::cross(T, edge1);

        // Avoid division by 0
        float a = glm::dot(P, edge1);
        if (a < EPSILON && a > -EPSILON) return false;

        float f = 1.0f / a;

        float u = glm::dot(P, T) * f;
        float v = glm::dot(Q, direction) * f;

        if (u + v > 1.0f || u < 0.0f || v < 0.0f) return false;

        float t = glm::dot(Q, edge2) * f;

        if (t > EPSILON) {

            if(!ray.intersection || ray.intersection->distanceToRayOrigin > t ){
                if(ray.intersection)
                    delete ray.intersection;

                ray.intersection = new Intersection(getPointOnTriangle(u, v), normal, material, t, ray.direction);
                return true;
            }
        }
        return false;
    }

    Vertex getPointOnTriangle(float u, float v) {
        return Vertex(v1.position + (u * edge1 + v * edge2));
    }

    bool operator==(const Triangle& other) const {
        return other.v1 == v1 && other.v2 == v2 && other.v3 == v3 && other.material->color == this->material->color && other.normal == normal;
    }

    bool operator!=(const Triangle& other) const {
        return !(&other == this);
    }

    Vertex v1, v2, v3;
    Material* material;
    Direction normal;
    float area;
    Material defaultMaterial = Material();

private:
    glm::vec3 edge1;
    glm::vec3 edge2;

    void computeNormal() {
        glm::vec3 normalVec = glm::normalize(glm::cross(edge1, edge2));
        normal = Direction(normalVec);
    }
};

struct MeshObject {
    MeshObject() : numTriangles(4) {
        triangles.resize(numTriangles);
    }

    MeshObject(int numTrianglesIn) : numTriangles(numTrianglesIn) {
        triangles.resize(numTriangles);
    }

    bool rayIntersection(Ray &ray) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].rayIntersection(ray);
        }
        return ray.intersection != nullptr;
    }

    int numTriangles;
    std::vector<Triangle> triangles;
};

struct Tetrahedron : MeshObject {
    Tetrahedron() : Tetrahedron(Vertex(glm::vec3(8.0, 3.0, -4.0)), new Material()) { }

    Tetrahedron(Vertex position, Material* material) : MeshObject(4) {
        Vertex v0 = Vertex(glm::vec3(1.0, 1.0, 1.0) + position.position);
        Vertex v1 = Vertex(glm::vec3(1.0, -1.0f, -1.0f) + position.position);
        Vertex v2 = Vertex(glm::vec3(-1.0f, 1.0, -1.0f) + position.position);
        Vertex v3 = Vertex(glm::vec3(-1.0f, -1.0f, 1.0) + position.position);

        // TODO: Arbitrarily chosen order of vertices - normal might be wrong.
        triangles[0] = Triangle(v0, v1, v2, material);
        triangles[1] = Triangle(v0, v1, v3, material);
        triangles[2] = Triangle(v0, v2, v3, material);
        triangles[3] = Triangle(v1, v2, v3, material);
    }
};

struct ImplicitSphere {

public:
    ImplicitSphere(float radiusIn, Vertex centerPos, Material* materialIn)
            : radius(radiusIn), radiusSquared(glm::pow(radiusIn, 2)), center(centerPos), material(materialIn) { }

    bool rayIntersection(Ray &ray) {
        float a = 1; // Dot product of rays direction with itself
        glm::vec3 dirRayOriginToCenter = ray.startPoint->position - center.position; //L
        float b = glm::dot((2.f * ray.direction.vector), dirRayOriginToCenter);
        float c = glm::dot(dirRayOriginToCenter, dirRayOriginToCenter) - radiusSquared;

        float d0, d1;
        if (!solveQuadratic(a, b, c, d0, d1)) return false;

        if (d0 > d1) std::swap(d0, d1);

        if (d0 < 0) {
            d0 = d1;
            if (d0 < 0) return false;
        }

        if(abs(d0) < EPSILON)
            return false;

        if(!ray.intersection || ray.intersection->distanceToRayOrigin > d0 ){
            if(ray.intersection)
                delete ray.intersection;

            Vertex intersectionPoint = Vertex(ray.startPoint->position + d0 * ray.direction.vector);
            ray.intersection = new Intersection(intersectionPoint,
                                                Direction(intersectionPoint.position - center.position), material, d0,
                                                ray.direction);
        }

        return true;
    }

    bool evaluate(glm::vec3 position) {
        return (glm::abs(glm::pow(glm::length(position - center.position), 2) - radiusSquared)) < EPSILON;
    }

    bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) {
        // Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
        float discr = b * b - 4 * a * c;
        if (discr < 0) return false;
        else if (glm::abs(discr) < EPSILON) x0 = x1 = - 0.5 * b / a;
        else {
            float q = (b > 0) ?
                      -0.5 * (b + sqrt(discr)) :
                      -0.5 * (b - sqrt(discr));
            x0 = q / a;
            x1 = c / q;
        }
        if (x0 > x1) std::swap(x0, x1);

        return true;
    }

private:
    float radius;
    float radiusSquared;
    Vertex center;
    Material* material;
};

struct Light {
    Light(Triangle& lightTriangleIn, ColorDbl colorIn)
            : color(colorIn), lightTriangle(lightTriangleIn) { }

    Vertex getRandomPointOnLight(){
        float random = glm::clamp((float) std::rand()/ RAND_MAX, 0.01f, 0.99f);
        float randomU = random / ((std::rand() % 8) + 2);
        float randomV = random - randomU;

        return lightTriangle.getPointOnTriangle(randomU, randomV);
    }

    ColorDbl color;
    Triangle lightTriangle;
};

