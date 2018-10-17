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

        glm::vec3 colorContrib = calculateLight(&root);
        ColorDbl pixelColor = ColorDbl(colorContrib);

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

        // In our case, we will only have either reflected or refracted light, never both.
        glm::vec3 currentLight = reflectedLight + refractedLight;

        // Calculate the contribution from the direct lighting
        if(currentNode->ray.intersection->material->type == LAMBERTIAN) {
            ColorDbl currentColorDbl = currentNode->ray.intersection->material->color;
            glm::vec3 currentColor = glm::vec3(currentColorDbl.r, currentColorDbl.g, currentColorDbl.b);

            glm::vec3 directLight = calculateDirectLight(currentNode->ray.intersection, &currentNode->ray);
            ColorDbl newColor = currentNode->ray.intersection->material->color * directLight;

            currentLight += (currentColor * directLight);
        }

        return currentLight;
    }

    Node createRayTree(Ray &ray) {
        Node root = Node(nullptr, ray, scene);
        Node* next = &root;

        int maxIterations = 10;
        for(int i = 0; i < maxIterations; i++) {
            if(next->ray.intersection->material->type == LAMBERTIAN)
                break;

            // TODO: ADD refracted rays as well.
            next->reflected = new Node(next, getReflectedRay(next->ray), scene);
            next = next->reflected;
        }
        return root;
    }

    glm::vec3 calculateDirectLight(Intersection* intersection, Ray* ray) {
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
