//
// Created by rebec on 2018-10-10.
//

#ifndef TNCG15_MC_RAYTRACER_RAYTRACE_H
#define TNCG15_MC_RAYTRACER_RAYTRACE_H
#include "Scene.h"

class RayTrace {
public:
    RayTrace(Scene* sceneIn) : scene(sceneIn){ }

    ColorDbl trace(Ray &ray){
        Node root = createRayTree(ray);

        // TODO: Go through the created ray tree and add the correct
        // TODO contributions (direct and indirect light)
        // TODO: Calculate direct light in leaves and propogate that up to the root

        // THIS SHOULD BE REPLACED BY COMPUTATIONS USING THE RAY TREE
        if (root.ray.intersection->material->type == LIGHT) return root.ray.intersection->material->color;
        glm::vec3 directLight = calculateDirectLight(root.ray.intersection, &root.ray);
        ColorDbl newColor = root.ray.intersection->material->color * directLight;
        if (newColor.g > 1.) {
         //   std::cout << "This is very green" << std::endl;
        }
        return newColor;
    }

private:
    struct Node {
        Node(Node* parentIn, Ray rayIn)
                : parent(parentIn), reflected(nullptr), refracted(nullptr), ray(rayIn) {}

        Node* parent;
        Node* reflected;
        Node* refracted;
        Ray ray;
    };

    Scene* scene;

    Node createRayTree(Ray &ray) {
        Node root = Node(nullptr, ray);
        scene->findIntersectedTriangle(root.ray);
       /* Node* next = &root;

        // TODO: Terminate ray by russian roulette if we are on diffuse surfaces
        // TODO: The ray should not be terminated on reflecting or refracting surfaces
        for(int i = 0; i < 3; i++) {
            // TODO: ADD refracted rays as well.
            next->reflected = new Node(next, getReflectedRay(next->ray));
            scene->findIntersectedTriangle(next->reflected->ray);
            next = next->reflected;
        }*/
        return root;
    }

    glm::vec3 calculateDirectLight(Intersection* intersection, Ray* ray) {
        // TODO: fix bugs
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

    Ray getReflectedRay(Ray &incomingRay) {
        // TODO: Introduce Monte Carlo scheme by using random directions (slide 209)
        glm::vec3 reflectedDir = glm::reflect(incomingRay.direction.vector, incomingRay.intersection->normal.vector);
        return Ray(&incomingRay.intersection->position, Direction(reflectedDir));
    }

    Ray getRefractedRay(){
        // TODO: Implement refracted rays
        return Ray();
    }

    glm::vec3 sendShadowRay(Light* light, Intersection* intersection, Ray* ray) {
        Vertex pointOnLight = light->getRandomPointOnLight();
        glm::vec3 s = pointOnLight.position - intersection->position.position;
        Vertex startPoint = Vertex(intersection->position.position + EPSILON * glm::normalize(s));
        Ray shadowRay = Ray(&startPoint, Direction(s));
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
