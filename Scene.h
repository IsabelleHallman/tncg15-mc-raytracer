//
// Created by Isabelle Hallman on 2018-09-17.
//

#ifndef TNCG15_MC_RAYTRACER_SCENE_H
#define TNCG15_MC_RAYTRACER_SCENE_H


#include <list>
#include "Geometry.h"

struct Light {
    Light(Triangle& areaLightIn, ColorDbl colorIn)
            : areaLight(areaLightIn), color(colorIn) { }

    ColorDbl color;
    Triangle areaLight;
};


class Scene {
public:
    Scene(std::list<Triangle>& trianglesIn)
            : walls(trianglesIn),
              sphere(ImplicitSphere(1, Vertex(glm::vec3(6.f, 3.f, -3.f)), ColorDbl(0.2, 0.2, 0.2))) { }

    Triangle& findIntersectedTriangle(Ray &ray) {
        for (auto iterator = walls.begin(); iterator != walls.end(); ++iterator) {
            iterator->rayIntersection(ray);
        }

        /*for (auto iterator = objects.begin(); iterator != objects.end(); ++iterator) {
            (iterator)->rayIntersection(ray);
        }*/

        if (ray.endPointTriangle != nullptr) return *ray.endPointTriangle;

        // TODO: Sometimes the ray hits inbetween two triangles (where it should not be any space), rounding error?
        ray.color = &defaultColor;
        return walls.front();
    }

    void addTetrahedron(Vertex position, ColorDbl color) {
        Tetrahedron tetrahedron = Tetrahedron(position, color);
        objects.push_back(tetrahedron);
    }

    void addLight(Triangle& areaLight, ColorDbl color) {
        lights.push_back(Light(areaLight, color));
    }

private:
    std::list<Light> lights;
    std::list<Triangle> walls;
    std::list<MeshObject> objects;
    ImplicitSphere sphere;

    ColorDbl defaultColor = ColorDbl(1.0, 1.0, 1.0);

};


#endif //TNCG15_MC_RAYTRACER_SCENE_H
