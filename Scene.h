//
// Created by Isabelle Hallman on 2018-09-17.
//

#ifndef TNCG15_MC_RAYTRACER_SCENE_H
#define TNCG15_MC_RAYTRACER_SCENE_H


#include <list>
#include "Geometry.h"

class Scene {
public:
    Scene(std::list<Triangle>& trianglesIn) : walls(trianglesIn) {
        std::list<ImplicitObject*> implicits;
        ImplicitCircle circle = ImplicitCircle();
        circle.evaluate();
        implicits.push_back(&circle);
        implicits.front()->evaluate();
    }

    Triangle& findIntersectedTriangle(Ray &ray) {
        for (auto iterator = walls.begin(); iterator != walls.end(); ++iterator) {
            iterator->rayIntersection(ray);
        }

        for (auto iterator = objects.begin(); iterator != objects.end(); ++iterator) {
            std::cout << "Evaluating object " << (*iterator)->numTriangles << std::endl;
            (*iterator)->rayIntersection(ray);
        }

        if (ray.endPointTriangle != nullptr) return *ray.endPointTriangle;
        return walls.front();
    }

    void addTetrahedron(Vertex position, ColorDbl color) {
        Tetrahedron tetrahedron = Tetrahedron(position, color);
        objects.emplace_back(&tetrahedron);
    }

private:
    std::list<Triangle> walls;
    std::list<std::unique_ptr<MeshObject>> objects;
};


#endif //TNCG15_MC_RAYTRACER_SCENE_H
