//
// Created by rebec on 2018-10-10.
//

#ifndef TNCG15_MC_RAYTRACER_RAYTRACE_H
#define TNCG15_MC_RAYTRACER_RAYTRACE_H
#include "Scene.h"
#include "glm/gtx/vector_angle.hpp"
#include <random>

class RayTrace {
public:
    RayTrace(Scene* sceneIn) : scene(sceneIn){
        std::random_device rd;
        gen = new std::mt19937(rd());
        dis = new std::uniform_real_distribution<float>(0, 1);
    }

    ~RayTrace(){
        delete gen;
        delete dis;
    }

    ColorDbl trace(Ray &ray){
        Node root = createRayTree(ray);

        glm::vec3 colorContrib = calculateLight(&root);
        ColorDbl pixelColor = ColorDbl(colorContrib);

        destroyNodeTree(&root);

        return pixelColor;
    }

private:
    struct Node {
        Node(Node* parentIn, Ray rayIn, Scene* scene)
                : parent(parentIn), reflected(nullptr), refracted(nullptr), ray(rayIn) {
            scene->findIntersectedTriangle(ray);
        }

        void calculateRadianceDistribution(float n1, float n2) {
            float R_0 = pow((n1 - n2)/(n1 + n2), 2);
            reflectionCoefficient = R_0 +
                    (1 - R_0) * pow(1 - glm::dot(ray.intersection->normal.vector, -1.f * ray.direction.vector), 5);
        }

        Node* parent;
        Node* reflected;
        Node* refracted;
        float reflectionCoefficient = 1.0f;
        Ray ray;
    };

    Scene* scene;
    const int MAX_DEPTH = 10;

    std::mt19937* gen;
    std::uniform_real_distribution<float>* dis;

    Node createRayTree(Ray &ray) {
        Node root = Node(nullptr, ray, scene);
        Node* next = &root;

        createNewRayNodes(&root, 0);

        return root;
    }

    void createNewRayNodes(Node* current_node, int depth){

        int material_type = current_node->ray.intersection->material->type;

        // Russian Roulette
        if(material_type == LAMBERTIAN || material_type == OREN_NAYAR){
            float random = std::rand()/RAND_MAX;
            float terminationProbability = (depth != 0) ? 0.2f : 0.0f;
            if(random < terminationProbability)
                 return;
        }

        float n1 = 0.f, n2 = 0.f;
        Ray refractedRay = Ray();
        switch(material_type){
            case PERFECT_REFLECTOR:
                current_node->reflected = new Node(current_node, getPerfectReflectedRay(current_node->ray), scene);
                createNewRayNodes(current_node->reflected, depth + 1);
                break;
            case TRANSPARENT:
                current_node->reflected = new Node(current_node, getPerfectReflectedRay(current_node->ray), scene);
                createNewRayNodes(current_node->reflected, depth + 1);

                if(!getRefractedRay(current_node->ray, n1, n2, refractedRay))
                    break;
                current_node->refracted = new Node(current_node, refractedRay, scene);
                current_node->calculateRadianceDistribution(n1, n2);
                createNewRayNodes(current_node->refracted, depth + 1);
                break;
            default :
                current_node->reflected = new Node(current_node, getRandomReflectedRay(current_node->ray), scene);
                createNewRayNodes(current_node->reflected, depth + 1);
                break;
        }
    }

    void destroyNodeTree(Node* root) {
        destroyNode(root->reflected);
        destroyNode(root->refracted);
    }

    void destroyNode(Node* node) {
        if (node) {
            destroyNode(node->reflected);
            destroyNode(node->refracted);
            delete node;
        }
    }

    glm::vec3 calculateLight(Node* currentNode) {
        glm::vec3 reflectedLight = glm::vec3(0.f),
                  refractedLight = glm::vec3(0.f);
        float refCoeff = currentNode->reflectionCoefficient;

        if(currentNode->reflected != nullptr)
            reflectedLight = refCoeff * calculateLight(currentNode->reflected);
        if(currentNode->refracted != nullptr)
            refractedLight = (1 - refCoeff) * calculateLight(currentNode->refracted);

        int matType = currentNode->ray.intersection->material->type;
        if (matType == LIGHT)
            return glm::vec3(1.f);

        glm::vec3 brdf = currentNode->ray.intersection->material->
                getBRDF(*currentNode->ray.intersection, currentNode->parent->ray.direction, currentNode->ray.direction);

        glm::vec3 indirectLight = glm::vec3(0.f),
                  directLight = glm::vec3(0.f);
        indirectLight = (reflectedLight + refractedLight) * brdf;

        // Calculate the contribution from the direct lighting
        if(matType != PERFECT_REFLECTOR && matType != TRANSPARENT)
            directLight = calculateDirectLight(&currentNode->ray);

        return (indirectLight + directLight);
    }

    glm::vec3 calculateDirectLight(Ray* ray) {
        glm::vec3 allLightsContributions = glm::vec3(0.0);
        int numRays = 1;

        for (auto iterator = scene->lightBegin(); iterator != scene->lightEnd(); ++iterator) {
            glm::vec3 singleLightContribution = glm::vec3(0.0);
            Light* light = &*iterator;
            for (int i = 0; i < numRays; i++)
                singleLightContribution += sendShadowRay(light, ray->intersection, ray);
            singleLightContribution *= (1.0 / numRays) * light->lightTriangle.area;
            allLightsContributions += singleLightContribution;
        }

        return allLightsContributions;
    }

    Ray getRandomReflectedRay(Ray &incomingRay) {
        glm::vec3 offset = 0.00001f * incomingRay.intersection->normal.vector;
        Vertex reflectedRayOrigin = Vertex(incomingRay.intersection->position.position + offset);

        Intersection intersection = *incomingRay.intersection;

        glm::vec3 helper = intersection.normal.vector + glm::vec3(1.f,1.f,1.f); // Basically only used to get a tangent
        glm::vec3 tangent = glm::normalize(glm::cross(intersection.normal.vector, helper));

        float rand1 = (*dis)(*gen);
        float rand2 = (*dis)(*gen);

        // Uniform distribution over a hemisphere
        float inclination = (float)acos(sqrt(rand1));
        float azimuth = (2.f * (float)M_PI * rand2);

        // Change the actual vector
        glm::vec3 random_direction = intersection.normal.vector;
        random_direction = glm::normalize(glm::rotate(
                random_direction,
                inclination,
                tangent));
        random_direction = glm::normalize(glm::rotate(
                random_direction,
                azimuth,
                intersection.normal.vector));

        return Ray(reflectedRayOrigin, Direction(random_direction));
    }

    Ray getPerfectReflectedRay(Ray &incomingRay) {
        glm::vec3 reflectedDir = glm::reflect(incomingRay.direction.vector, incomingRay.intersection->normal.vector);

        glm::vec3 offset = 0.00001f * incomingRay.intersection->normal.vector;
        Vertex reflectedRayOrigin = Vertex(incomingRay.intersection->position.position + offset);

        return Ray(reflectedRayOrigin, Direction(reflectedDir));
    }

    bool getRefractedRay(Ray &incomingRay, float &n1, float &n2, Ray& outgoingRay) {
        n1 = 1.0; // AIR
        n2 = incomingRay.intersection->material->refractionIndex;

        glm::vec3 surfaceNormal = incomingRay.intersection->normal.vector;

        // Check if we are inside of the object or outside, swap if we are inside
        float angle = glm::angle(surfaceNormal, incomingRay.direction.vector);
        if(angle > (float)M_PI/2.f){
            surfaceNormal*= -1.f;
            std::swap(n1,n2);
        }

        if(n1 > n2){
            float brewsterangle = glm::asin(n2/n1);
            angle = glm::angle(surfaceNormal, incomingRay.direction.vector);
            if(angle > brewsterangle)
                return false;
        }

        float refractionRatio = n1/n2;
        glm::vec3 refractedRay = refract(incomingRay.direction.vector, surfaceNormal, refractionRatio);

        glm::vec3 offset = 0.00001f * surfaceNormal;
        Vertex refractedRayOrigin = Vertex(incomingRay.intersection->position.position + offset);

        outgoingRay = Ray(refractedRayOrigin, Direction(refractedRay));
        return true;
    }

    glm::vec3 sendShadowRay(Light* light, Intersection* intersection, Ray* ray) {
        Vertex pointOnLight = light->getRandomPointOnLight();
        glm::vec3 s = pointOnLight.position - intersection->position.position;
        Vertex startPoint = Vertex(intersection->position.position + EPSILON * glm::normalize(s));
        Ray shadowRay = Ray(startPoint, Direction(s));
        if (!scene->findIntersectedTriangle(shadowRay) || shadowRay.intersection->position != pointOnLight)
            return glm::vec3(0.0);

        float d = glm::length(s);
        //s = glm::normalize(s);
        float cosAlpha = - glm::dot(s, light->lightTriangle.normal.vector);
        float cosBeta = glm::dot(s, intersection->normal.vector);

        float g = glm::abs(cosAlpha) * glm::abs(cosBeta) / (d * d);

        glm::vec3 brdfResult = intersection->material->getBRDF(*intersection, shadowRay.direction, ray->direction);

        return g * brdfResult;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
