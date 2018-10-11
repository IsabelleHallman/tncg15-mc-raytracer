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
        //Node root = createRayTree(ray);
       // Node* next = &root;
        ColorDbl color = ColorDbl(0,0,0);
        Node root = Node(nullptr, ray);
        scene->findIntersectedTriangle(root.ray);
        /*while(next != nullptr) {
            color += *(next->ray.color);
            next = next->reflected;
        }*/
        if(!isInShadow(root))
            return *(root.ray.color);
        return color;
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
    bool isInShadow(Node& node){
        Light* light = scene->getLight();
        Ray shadowRay = Ray( &node.ray.endPoint,
                             Direction(node.ray.endPoint.position - light->getRandomPointOnLight().position));

        scene->findIntersectedTriangle(shadowRay);

        // If the shadow ray intersects with the light, the node (the original ray's end point) is in light
        if (shadowRay.endPointTriangle && *(shadowRay.endPointTriangle) == light->areaLight) {
            return false;
        }
        // Otherwise there is another object in the way and the point is in shadow.


        return true;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
