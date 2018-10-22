//
// Created by rebec on 2018-10-10.
//

#ifndef TNCG15_MC_RAYTRACER_RAYTRACE_H
#define TNCG15_MC_RAYTRACER_RAYTRACE_H
#include "Scene.h"
#include "glm/gtx/vector_angle.hpp"

class RayTrace {
public:
    RayTrace(Scene* sceneIn) : scene(sceneIn){ }

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

        Node* parent;
        Node* reflected;
        Node* refracted;
        Ray ray;
    };

    Scene* scene;

    void destroyNodeTree(Node* root){
        destroyNode(root->reflected);
        destroyNode(root->refracted);
    }

    void destroyNode(Node* node){
        if (node)
        {
            destroyNode(node->reflected);
            destroyNode(node->refracted);
            delete node;
        }
    }

    glm::vec3 calculateLight(Node* currentNode) {
        glm::vec3 reflectedLight = glm::vec3(0.f), refractedLight = glm::vec3(0.f);

        if(currentNode->reflected != nullptr)
            reflectedLight = calculateLight(currentNode->reflected);
        if(currentNode->refracted != nullptr)
            refractedLight = calculateLight(currentNode->refracted);

        if (currentNode->ray.intersection->material->type == LIGHT) {
            ColorDbl lightColor = currentNode->ray.intersection->material->color;
            return glm::vec3(lightColor.r, lightColor.g, lightColor.b);
        }

        // TODO: USE SCHLICKS EQ. TO KNOW HOW MUCH OF EACH SHOULD BE REPRESENTED
        glm::vec3 currentLight = reflectedLight + refractedLight;

        // Calculate the contribution from the direct lighting
        if(currentNode->ray.intersection->material->type == LAMBERTIAN) {
            ColorDbl currentColorDbl = currentNode->ray.intersection->material->color;
            glm::vec3 currentColor = glm::vec3(currentColorDbl.r, currentColorDbl.g, currentColorDbl.b);

            glm::vec3 directLight = calculateDirectLight(&currentNode->ray);

            currentLight += (currentColor * directLight);
        }

        return currentLight;
    }

    // TODO: Change to recursive so each node can have both reflected and refracted rays
    Node createRayTree(Ray &ray) {
        Node root = Node(nullptr, ray, scene);
        Node* next = &root;

        int maxIterations = 10;
        int iter = 0;

        while(next != nullptr){
            if(next->ray.intersection->material->type == LAMBERTIAN)
                break;

            if(iter == maxIterations){
                break;
            }

            if(next->ray.intersection->material->type == PERFECT_REFLECTOR) {
                next->reflected = new Node(next, getReflectedRay(next->ray), scene);
                next = next->reflected;
            }
            if(next->ray.intersection->material->type == TRANSPARENT){
                next->refracted = new Node(next, getRefractedRay(next->ray), scene);
                next = next->refracted;
            }
            iter++;
        }
        return root;
    }

    glm::vec3 calculateDirectLight(Ray* ray) {
        glm::vec3 allLightsContributions = glm::vec3(0.0);
        int numRays = 3;

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

    // TODO: Introduce Monte Carlo scheme by using random directions (slide 209)
    Ray getReflectedRay(Ray &incomingRay) {
        glm::vec3 reflectedDir = glm::reflect(incomingRay.direction.vector, incomingRay.intersection->normal.vector);

        glm::vec3 offset = 0.00001f * incomingRay.intersection->normal.vector;
        Vertex reflectedRayOrigin = Vertex(incomingRay.intersection->position.position + offset);

        return Ray(reflectedRayOrigin, Direction(reflectedDir));
    }

    Ray getRefractedRay(Ray &incomingRay){
        float n1 = 1.0; // AIR
        float n2 = incomingRay.intersection->material->refractionIndex;

        glm::vec3 surfaceNormal = incomingRay.intersection->normal.vector;
        glm::vec3 offset = 0.00001f * surfaceNormal;

        // Check if we are inside of the object or outside, swap if we are inside
        float angle = glm::angle(incomingRay.direction.vector, surfaceNormal);
        if(angle < glm::pi<float>()/2.f){
            surfaceNormal*= -1.f;
            offset *= -1.f;
            std::swap(n1,n2);
        }

        // TODO: CHANGE THIS! IF THE ANGLE IS TOO LARGE, THERE SHOULD BE NO REFRACTION RAY
        /*if(n1 > n2){
            float brewsterAngle = glm::asin(n2 / n1);
            float alpha = glm::angle(incomingRay.direction.vector, surfaceNormal);
            if(alpha > brewsterAngle)
                return Ray();
        }*/

        float refractionRatio = n1/n2;
        glm::vec3 refractedRay = refract(incomingRay.direction.vector, surfaceNormal, refractionRatio);
        Vertex refractedRayOrigin = Vertex(incomingRay.intersection->position.position + offset);

        return Ray(refractedRayOrigin, Direction(refractedRay));
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

        glm::vec3 brdfResult = intersection->material->getBRDF(intersection->position, shadowRay.direction, ray->direction);

        return g * brdfResult;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
