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

        // THIS SHOULD BE REPLACED BY COMPUTATIONS USING THE RAY TREE
        if(isInShadow(root.ray.intersection->position))
            return ColorDbl(0,0,0);
        return root.ray.intersection->material->color;
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

    Node createRayTree(Ray &ray){
        Node root = Node(nullptr, ray);
        scene->findIntersectedTriangle(root.ray);
        Node* next = &root;

        // TODO: Terminate ray by russian roulette if we are on diffuse surfaces
        // TODO: The ray should not be terminated on reflecting or refracting surfaces
        for(int i = 0; i < 3; i++) {
            // TODO: ADD refracted rays as well.
            next->reflected = new Node(next, getReflectedRay(next->ray));
            scene->findIntersectedTriangle(next->reflected->ray);
            next = next->reflected;
        }
        return root;
    }

    Ray getReflectedRay(Ray &incomingRay){
        // TODO: Introduce Monte Carlo scheme by using random directions (slide 209)
        glm::vec3 reflectedDir = glm::reflect(incomingRay.direction.vector, incomingRay.intersection->normal.vector);
        return Ray(&incomingRay.intersection->position, Direction(reflectedDir));
    }

    Ray getRefractedRay(){
        // TODO: Implement refracted rays
        return Ray();
    }

    // Send shadow rays
    bool isInShadow(Vertex& intersectionPoint){
        Light* light = scene->getLight();
        float random = glm::clamp((float) std::rand()/ RAND_MAX, 0.01f, 0.99f);
        float random1 = random / ((std::rand() % 8) + 2);
        Vertex pointOnLight = (light->areaLight).getPointOnTriangle(random1, random - random1);

        Ray shadowRay = Ray(&intersectionPoint, Direction(pointOnLight.position - intersectionPoint.position));

        scene->findIntersectedTriangle(shadowRay);

        if(shadowRay.intersection && shadowRay.intersection->position == pointOnLight)
            return false;

        return true;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
