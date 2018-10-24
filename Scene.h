//
// Created by Isabelle Hallman on 2018-09-17.
//

#ifndef TNCG15_MC_RAYTRACER_SCENE_H
#define TNCG15_MC_RAYTRACER_SCENE_H


#include <list>
#include <vector>
#include "Utility.h"


class Scene {
public:
    Scene(std::list<Triangle>& trianglesIn)
            : walls(trianglesIn), defaultMaterial(LambertianMaterial(glm::vec3(0.f), glm::vec3(0.f))) { }

    Scene() : defaultMaterial(LambertianMaterial(glm::vec3(1.f), glm::vec3(1.f))) { };

    bool findIntersectedTriangle(Ray &ray) {
        for (auto iterator = walls.begin(); iterator != walls.end(); ++iterator) {
            iterator->rayIntersection(ray);
        }

        for (auto iterator = objects.begin(); iterator != objects.end(); ++iterator) {
            (iterator)->rayIntersection(ray);
        }

        for (auto iterator = implicitSpheres.begin(); iterator != implicitSpheres.end(); ++iterator){
            (iterator)->rayIntersection(ray);
        }

        for (auto iterator = lights.begin(); iterator != lights.end(); ++iterator) {
            (iterator)->rayIntersection(ray);
        }

        // TODO: Sometimes the ray hits inbetween two triangles (where it should not be any space), rounding error?
        if(!ray.intersection) {
            ray.intersection = new Intersection(Vertex(), Direction(), &defaultMaterial, 10000);
            return false;
        }
        return true;
    }

    void addWalls(std::list<Triangle>& trianglesIn) {
        walls = trianglesIn;
    }

    void addTetrahedron(Vertex position, Material* material) {
        Tetrahedron tetrahedron = Tetrahedron(position, material);
        objects.push_back(tetrahedron);
    }

    void addBox(Vertex position, Material* material, float width, float depth, float height) {
        Box box = Box(position, material, width, depth, height);
        objects.push_back(box);
    }

    void addImplicitSphere(float radius, Vertex position, Material* material) {
        ImplicitSphere sphere = ImplicitSphere(radius, position, material);
        implicitSpheres.push_back(sphere);
    }

    int addMaterial(Material material) {
        materials.push_back(material);
        return materials.size() - 1;
    }

    void addLight(Triangle& areaLight, ColorDbl color) {
        lights.push_back(Light(areaLight, color, 1000));
    }

    Material* getMaterial(int index) {
        return &(materials.at(index));
    }

    std::list<Light>::iterator lightBegin() {
        return lights.begin();
    }

    std::list<Light>::iterator lightEnd() {
        return lights.end();
    }

private:

    std::list<Triangle> walls;
    std::list<MeshObject> objects;
    std::list<ImplicitSphere> implicitSpheres;
    std::list<Light> lights;

    std::vector<Material> materials;
    LambertianMaterial defaultMaterial;
};


#endif //TNCG15_MC_RAYTRACER_SCENE_H
