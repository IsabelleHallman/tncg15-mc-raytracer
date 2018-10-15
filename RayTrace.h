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
        //Ray rootRay = root.ray;
        //Ray rootRay = ray;
        //scene->findIntersectedTriangle(rootRay);
        //if(isInShadow(rootRay.endPoint))
          //  return ColorDbl(0,0,0);
        return *(root.ray.color);
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
        /*Node* next = &root;

        for(int i = 0; i < 3; i++) {
            // TODO: Stop when hits diffuse surface or when max depth is reached
            next->reflected = new Node(next, getReflectedRay(next->ray));
            scene->findIntersectedTriangle(next->reflected->ray);
            next = next->reflected;
        }*/
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
        float random = glm::clamp((float) std::rand()/ RAND_MAX, 0.01f, 0.99f);
        float random1 = random / ((std::rand() % 8) + 2);
        Vertex pointOnLight = (light->areaLight).getPointOnTriangle(random1, random - random1);

        Ray shadowRay = Ray(&intersectionPoint, Direction(pointOnLight.position - intersectionPoint.position));

        scene->findIntersectedTriangle(shadowRay);

        if(shadowRay.endPoint == pointOnLight ||
                shadowRay.endPointTriangle && *shadowRay.endPointTriangle == light->areaLight) {
            return false;
        }
        //if(shadowRay.startPoint->position.z < -4.9)
          //  std::cout << "here";

        return true;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
