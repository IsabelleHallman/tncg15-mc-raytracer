//
// Created by rebec on 2018-10-10.
//

#ifndef TNCG15_MC_RAYTRACER_RAYTRACE_H
#define TNCG15_MC_RAYTRACER_RAYTRACE_H
#include "Scene.h"

class RayTrace
{
public:
    RayTrace(Scene* sceneIn) : scene(sceneIn){ }

    ColorDbl trace(Ray &ray){
        Node root = createRayTree(ray);

        // TODO: Go through the created ray tree and add the correct
        // TODO contributions (direct and indirect light)
        // TODO: Calculate direct light in leaves and propogate that up to the root

        Light* light = scene->getLight();

        // THIS SHOULD BE REPLACED BY COMPUTATIONS USING THE RAY TREE
        glm::vec3 sum = glm::vec3(0.0);
        for (int i = 0; i < 10; i++)
            sum += sendShadowRay(light, root.ray.intersection, &root.ray);
        sum *= 0.1 * light->lightTriangle.area;
        return root.ray.intersection->material->color * sum;
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

        Ray shadowRay = Ray(&intersection->position, Direction(s));
        if (!scene->findIntersectedTriangle(shadowRay) || shadowRay.intersection->position != pointOnLight)
            return glm::vec3(0.0);

        float d = glm::length(s);
        //s = glm::normalize(s);
        float cosAlpha = - glm::dot(s, light->lightTriangle.normal.vector);
        float cosBeta = glm::dot(s, intersection->normal.vector);

        float g = glm::abs(cosAlpha) * glm::abs(cosBeta) / (d * d);

        if (g < 0)
            std::cout << "it's negative" << std::endl;

        glm::vec3 brdfResult = intersection->material->getBRDF(intersection->position, shadowRay.direction, ray->direction);

        return g * brdfResult;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
