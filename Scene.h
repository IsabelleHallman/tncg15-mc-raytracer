//
// Created by Isabelle Hallman on 2018-09-17.
//

#ifndef TNCG15_MC_RAYTRACER_SCENE_H
#define TNCG15_MC_RAYTRACER_SCENE_H


#include <list>
#include "Geometry.h"

class Scene {
public:
    Scene(std::list<Triangle>& trianglesIn) : triangles(trianglesIn) { }

    Triangle& findIntersectedTriangle(Ray &ray) {
        for (auto iterator = triangles.begin(); iterator != triangles.end(); ++iterator) {
            if (iterator->rayIntersection(ray)) {
                return *iterator;
            }
        }
        return triangles.front();
    }

private:
    std::list<Triangle> triangles;

};


#endif //TNCG15_MC_RAYTRACER_SCENE_H
