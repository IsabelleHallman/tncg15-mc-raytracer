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
        Ray rootRay = root.ray;
        if(isInShadow(rootRay.endPoint))
            return ColorDbl(0,0,0);
        return *(rootRay.color);
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

        for(int i = 0; i < 3; i++) {
            next->reflected = new Node(next, getReflectedRay(next->ray));
            scene->findIntersectedTriangle(next->reflected->ray);
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

    // Send shadow rays
    bool isInShadow(Vertex& intersectionPoint){
        Light* light = scene->getLight();
        //float random = std::rand()/ RAND_MAX;
        Vertex pointOnLight = (light->areaLight).getPointOnTriangle(0.5, 1-0.5);

        Ray shadowRay = Ray(&intersectionPoint, Direction(pointOnLight.position - intersectionPoint.position));

        scene->findIntersectedTriangle(shadowRay);

        if(shadowRay.endPoint == pointOnLight ||
                shadowRay.endPointTriangle && *shadowRay.endPointTriangle == light->areaLight) {
            return false;
        }
        if(shadowRay.startPoint->position.z < -4.9)
            std::cout << "here";

        return true;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
