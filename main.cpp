//
// Created by Isabelle Hallman on 2018-09-17.
//

#include "Camera.h"
#include <list>
#include <iostream>
#include <vector>

Scene generateTestScene() {
    std::vector<Vertex> sceneVertices =
            {
                    Vertex(0.0, 6.0, 5.0, 1.0),
                    Vertex(0.0, 6.0, -5.0f, 1.0),
                    Vertex(10.0, 6.0, 5.0, 1.0),
                    Vertex(10.0, 6.0, -5.0f, 1.0),
                    Vertex(13.0, 0.0, 5.0, 1.0),
                    Vertex(13.0, 0.0, -5.0f, 1.0),
                    Vertex(10.0, -6.0f, 5.0, 1.0),
                    Vertex(10.0, -6.0f, -5.0f, 1.0),
                    Vertex(0.0, -6.0f, 5.0, 1.0),
                    Vertex(0.0, -6.0f, -5.0f, 1.0),
                    Vertex(-3.0f, 0.0, 5.0, 1.0),
                    Vertex(-3.0f, 0.0, -5.0f, 1.0),
                    Vertex(5.0, 0.0, 5.0, 1.0),
                    Vertex(5.0, 0.0, -5.0f, 1.0)
            };

    ColorDbl redColor = ColorDbl(1.0, 0.0, 0.0);
    Material red = Material(redColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));

    ColorDbl greenColor = ColorDbl(0.0, 1.0, 0.0);
    Material green = Material(greenColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));

    ColorDbl blueColor = ColorDbl(0.0, 0.0, 1.0);
    Material blue = Material(blueColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));

    ColorDbl yellowColor = ColorDbl(1.0, 1.0, 0.0);
    Material yellow = Material(yellowColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));

    ColorDbl magentaColor = ColorDbl(1.0, 0.0, 1.0);
    Material magenta = Material(magentaColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));

    ColorDbl cyanColor = ColorDbl(0.0, 1.0, 1.0);
    Material cyan = Material(cyanColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));

    ColorDbl whiteColor = ColorDbl(1.0, .9, 1.0);
    Material white = Material(whiteColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));

    Material light = Material(whiteColor, 1.0, 0.0, LIGHT, glm::vec3(1.0));

    Material perfectReflector = Material(whiteColor, 1.0, 0.0, PERFECT_REFLECTOR, glm::vec3(0.5));

    Scene scene = Scene();

    int redIndex = scene.addMaterial(red);
    int greenIndex = scene.addMaterial(green);
    int blueIndex = scene.addMaterial(blue);
    int yellowIndex = scene.addMaterial(yellow);
    int magentaIndex = scene.addMaterial(magenta);
    int cyanIndex = scene.addMaterial(cyan);
    int whiteIndex = scene.addMaterial(white);
    int perfectReflectorIndex = scene.addMaterial(perfectReflector);
    int lightIndex = scene.addMaterial(light);

    ColorDbl tetraColor = ColorDbl(.5, 1., 1.);
    Material tetraMaterial = Material(tetraColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));
    int tetraMaterialIndex = scene.addMaterial(tetraMaterial);

    // Moving these lines below sceneTriangles creation causes weird errors on the scene materials. Memory conflict?
    Material sphereMaterial = Material(redColor, 1.0, 0.0, LAMBERTIAN, glm::vec3(1.0));
    int sphereMaterialIndex = scene.addMaterial(sphereMaterial);

    Material mirrorMaterial = Material(whiteColor, 1.0, 0.0, PERFECT_REFLECTOR, glm::vec3(1.0));
    int mirrorMaterialIndex = scene.addMaterial(mirrorMaterial);

    std::list<Triangle> sceneTriangles = {
            Triangle(sceneVertices.at(0), sceneVertices.at(2), sceneVertices.at(1), scene.getMaterial(redIndex)),
            Triangle(sceneVertices.at(2), sceneVertices.at(1), sceneVertices.at(3), scene.getMaterial(redIndex)),
            Triangle(sceneVertices.at(3), sceneVertices.at(2), sceneVertices.at(4), scene.getMaterial(greenIndex)),
            Triangle(sceneVertices.at(3), sceneVertices.at(4), sceneVertices.at(5), scene.getMaterial(greenIndex)),
            Triangle(sceneVertices.at(4), sceneVertices.at(6), sceneVertices.at(5), scene.getMaterial(blueIndex)),
            Triangle(sceneVertices.at(5), sceneVertices.at(6), sceneVertices.at(7), scene.getMaterial(blueIndex)),
            Triangle(sceneVertices.at(6), sceneVertices.at(8), sceneVertices.at(7), scene.getMaterial(yellowIndex)),
            Triangle(sceneVertices.at(7), sceneVertices.at(8), sceneVertices.at(9), scene.getMaterial(yellowIndex)),
            Triangle(sceneVertices.at(8), sceneVertices.at(10), sceneVertices.at(9), scene.getMaterial(magentaIndex)),
            Triangle(sceneVertices.at(9), sceneVertices.at(10), sceneVertices.at(11), scene.getMaterial(magentaIndex)),
            Triangle(sceneVertices.at(10), sceneVertices.at(0), sceneVertices.at(11), scene.getMaterial(cyanIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(1), sceneVertices.at(11), scene.getMaterial(cyanIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(10), sceneVertices.at(8), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(8), sceneVertices.at(12), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(12), sceneVertices.at(2), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(2), sceneVertices.at(12), sceneVertices.at(6), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(12), sceneVertices.at(8), sceneVertices.at(6), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(2), sceneVertices.at(6), sceneVertices.at(4), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(11), sceneVertices.at(1), sceneVertices.at(9), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(1), sceneVertices.at(13), sceneVertices.at(9), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(1), sceneVertices.at(3), sceneVertices.at(13), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(13), sceneVertices.at(3), sceneVertices.at(7), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(13), sceneVertices.at(7), sceneVertices.at(9), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(3), sceneVertices.at(5), sceneVertices.at(7), scene.getMaterial(whiteIndex))
    };

    scene.addWalls(sceneTriangles);

    // Tetrahedron Objects
    Vertex tetraPosition = Vertex(glm::vec3(8.0, 3.0, -2.0));
    scene.addTetrahedron(tetraPosition, scene.getMaterial(tetraMaterialIndex));

    Vertex tetraPosition2 = Vertex(glm::vec3(8.0, 0.0, 0.0));
    scene.addTetrahedron(tetraPosition2, scene.getMaterial(tetraMaterialIndex));

    // Implicit spheres
    Vertex centerOfSphere = Vertex(glm::vec3(6.f, -2.f, -3.f));
    scene.addImplicitSphere(1.0, centerOfSphere, scene.getMaterial(mirrorMaterialIndex));

    Vertex centerOfSphere2 = Vertex(glm::vec3(8.f, 3.f, 4.f));
    scene.addImplicitSphere(1.0, centerOfSphere2, scene.getMaterial(sphereMaterialIndex));

    Vertex l0 = Vertex(5.0, -2.0f, 4.9, 1.0);
    Vertex l1 = Vertex(7.0f, 0.0, 4.9, 1.0);
    Vertex l2 = Vertex(5.0, 2.0f, 4.9, 1.0);
    Triangle lightTriangle = Triangle(l0, l2, l1, scene.getMaterial(lightIndex));
    scene.addLight(lightTriangle, whiteColor);

    Vertex l3 = Vertex(1.0, -3.0f, 4.95, 1.0);
    Vertex l4 = Vertex(3.0f, 0.0, 4.95, 1.0);
    Vertex l5 = Vertex(1.0, 1.0f, 4.95, 1.0);
    Triangle lightTriangle2 = Triangle(l3, l5, l4, scene.getMaterial(lightIndex));
    //scene.addLight(lightTriangle2, whiteColor);

    Vertex l6 = Vertex(1.0, -3.0f, -4.95f, 1.0);
    Vertex l7 = Vertex(3.0f, 0.0, -4.95f, 1.0);
    Vertex l8 = Vertex(1.0, 1.0f, -4.95f, 1.0);
    Triangle lightTriangle3 = Triangle(l6, l7, l8, scene.getMaterial(lightIndex));
    scene.addLight(lightTriangle3, whiteColor);

    Vertex l9 = Vertex(5.0, -2.0f, -4.9f, 1.0);
    Vertex l10 = Vertex(7.0f, 0.0, -4.9f, 1.0);
    Vertex l11 = Vertex(5.0, 2.0f, -4.9f, 1.0);
    Triangle lightTriangle4 = Triangle(l9, l11, l10, scene.getMaterial(lightIndex));
    //scene.addLight(lightTriangle, whiteColor);

    return scene;
}

int main() {
    Vertex v1 = Vertex(.0, .0, .0, .0);
    Vertex v2 = Vertex(.0, .0, .1, .1);
    Vertex v3 = Vertex(.1, .0, .0, .2);
    ColorDbl color = ColorDbl(1., 1., 1.);

    Scene myScene = generateTestScene();

    Camera camera = Camera(&myScene);
    camera.render();
    camera.createImage();
    return 0;
}

