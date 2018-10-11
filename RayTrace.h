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
        Node* next = &root;
        ColorDbl color = ColorDbl(0,0,0);
        while(next != nullptr) {
            color += *(next->ray.color);
            next = next->reflected;
        }
        return color/3.f;
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
        root.reflected = new Node(&root, getReflectedRay(root.ray));
        Node* next = root.reflected;

        for(int i = 0; i < 3; i++) {
            scene->findIntersectedTriangle(next->ray);
            next->reflected = new Node(next, getReflectedRay(next->ray));
            next = next->reflected;
        }
        return root;
    }

    Ray getReflectedRay(Ray &incomingRay){
        glm::vec3 reflectedDir = glm::reflect(incomingRay.direction.vector, incomingRay.normal.vector);
        return Ray(&incomingRay.endPoint, Direction(reflectedDir));
    }

    Ray getRefractedRay(){
        return Ray();
    }

    void sendShadowRays(){}
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
