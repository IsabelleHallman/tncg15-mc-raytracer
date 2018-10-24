//
// Created by Isabelle Hallman on 2018-09-17.
//

#include "Camera.h"
#include <list>
#include <iostream>
#include <vector>
#include <chrono>


Scene generateTestScene();

int main() {
    auto startTime = std::chrono::system_clock::now();
    Scene myScene = generateTestScene();

    Camera camera = Camera(&myScene);
    camera.render();
    camera.createImage();

    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> diffTime = endTime - startTime;

    int timeInTakenInSec = (int)diffTime.count();

    int minutes = timeInTakenInSec / 60;
    int seconds = timeInTakenInSec % 60;

    std::cout << minutes << " minutes and " << seconds << " seconds";

    return 0;
}


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
    Material red = LambertianMaterial(redColor, glm::vec3(redColor.r, redColor.g, redColor.b));

    ColorDbl greenColor = ColorDbl(0.0, 1.0, 0.0);
    Material green = LambertianMaterial(greenColor, glm::vec3(0.f, 1.0, 0.f));

    ColorDbl blueColor = ColorDbl(0.0, 0.0, 1.0);
    Material blue = LambertianMaterial(blueColor, glm::vec3(0.f, 0.f, 1.0));

    ColorDbl yellowColor = ColorDbl(1.0, 1.0, 0.0);
    Material yellow = LambertianMaterial(yellowColor, glm::vec3(1.0, 1.f, 0.f));

    ColorDbl magentaColor = ColorDbl(1.0, 0.0, 1.0);
    Material magenta = LambertianMaterial(magentaColor, glm::vec3(1.0, 0.f, 1.f));

    ColorDbl cyanColor = ColorDbl(0.0, 1.0, 1.0);
    Material cyan = LambertianMaterial(cyanColor, glm::vec3(0.f, 1.0, 1.f));

    ColorDbl whiteColor = ColorDbl(1.0, .9, 1.0);
    Material white = LambertianMaterial(whiteColor, glm::vec3(1.0, 0.9f, 1.f));

    Material light = LightMaterial(whiteColor);

    Material transparent = Material(TRANSPARENT, 1.5f);

    Scene scene = Scene();

    int redIndex = scene.addMaterial(red);
    int greenIndex = scene.addMaterial(green);
    int blueIndex = scene.addMaterial(blue);
    int yellowIndex = scene.addMaterial(yellow);
    int magentaIndex = scene.addMaterial(magenta);
    int cyanIndex = scene.addMaterial(cyan);
    int whiteIndex = scene.addMaterial(white);
    int lightIndex = scene.addMaterial(light);
    int transparentIndex = scene.addMaterial(transparent);

    ColorDbl tetraColor = ColorDbl(.5, 1., 1.);
    Material tetraMaterial = LambertianMaterial(tetraColor, glm::vec3(.5f, 1.f, 1.0));
    int tetraMaterialIndex = scene.addMaterial(tetraMaterial);

    // Moving these lines below sceneTriangles creation causes weird errors on the scene materials. Memory conflict?
    Material sphereMaterial = OrenNayarMaterial(redColor, glm::vec3(1.0, 0.f, 0.f), 5.0);
    int sphereMaterialIndex = scene.addMaterial(sphereMaterial);

    Material mirrorMaterial = PerfectReflectorMaterial();
    int mirrorMaterialIndex = scene.addMaterial(mirrorMaterial);

    std::list<Triangle> sceneTriangles = {
            Triangle(sceneVertices.at(1), sceneVertices.at(2), sceneVertices.at(0), scene.getMaterial(redIndex)),
            Triangle(sceneVertices.at(1), sceneVertices.at(3), sceneVertices.at(2), scene.getMaterial(redIndex)),
            Triangle(sceneVertices.at(2), sceneVertices.at(3), sceneVertices.at(4), scene.getMaterial(mirrorMaterialIndex)),
            Triangle(sceneVertices.at(4), sceneVertices.at(3), sceneVertices.at(5), scene.getMaterial(mirrorMaterialIndex)),
            Triangle(sceneVertices.at(4), sceneVertices.at(5), sceneVertices.at(6), scene.getMaterial(blueIndex)),
            Triangle(sceneVertices.at(6), sceneVertices.at(5), sceneVertices.at(7), scene.getMaterial(blueIndex)),
            Triangle(sceneVertices.at(6), sceneVertices.at(7), sceneVertices.at(8), scene.getMaterial(yellowIndex)),
            Triangle(sceneVertices.at(8), sceneVertices.at(7), sceneVertices.at(9), scene.getMaterial(yellowIndex)),
            Triangle(sceneVertices.at(8), sceneVertices.at(9), sceneVertices.at(10), scene.getMaterial(magentaIndex)),
            Triangle(sceneVertices.at(10), sceneVertices.at(9), sceneVertices.at(11), scene.getMaterial(magentaIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(10), sceneVertices.at(11), scene.getMaterial(cyanIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(11), sceneVertices.at(1), scene.getMaterial(cyanIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(8), sceneVertices.at(10), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(12), sceneVertices.at(8), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(0), sceneVertices.at(2), sceneVertices.at(12), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(2), sceneVertices.at(6), sceneVertices.at(12), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(12), sceneVertices.at(6), sceneVertices.at(8), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(2), sceneVertices.at(4), sceneVertices.at(6), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(11), sceneVertices.at(9), sceneVertices.at(1), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(1), sceneVertices.at(9), sceneVertices.at(13), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(1), sceneVertices.at(13), sceneVertices.at(3), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(13), sceneVertices.at(7), sceneVertices.at(3), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(13), sceneVertices.at(9), sceneVertices.at(7), scene.getMaterial(whiteIndex)),
            Triangle(sceneVertices.at(3), sceneVertices.at(7), sceneVertices.at(5), scene.getMaterial(whiteIndex))
    };

    scene.addWalls(sceneTriangles);

    // Tetrahedron Objects
    Vertex tetraPosition = Vertex(glm::vec3(9.0, -2.0, 1.0));
    scene.addTetrahedron(tetraPosition, scene.getMaterial(tetraMaterialIndex));

    //Box objects
    Vertex boxPosition = Vertex(glm::vec3(6.0, -4.0, -5.0));
    scene.addBox(boxPosition, scene.getMaterial(redIndex), 3.0, 3.0, 3.0);

    // Implicit spheres
    Vertex centerOfSphere = Vertex(glm::vec3(6.f, -4.f, 3.f));
    scene.addImplicitSphere(0.5, centerOfSphere, scene.getMaterial(mirrorMaterialIndex));

    Vertex centerOfSphere2 = Vertex(glm::vec3(8.f, 3.f, 2.f));
    scene.addImplicitSphere(1.2, centerOfSphere2, scene.getMaterial(greenIndex));

    Vertex centerOfSphere3 = Vertex(glm::vec3(6.f, 3.f, -2.f));
    scene.addImplicitSphere(1.0, centerOfSphere3, scene.getMaterial(mirrorMaterialIndex));

    Vertex l0 = Vertex(5.0, -2.0f, 4.99, 1.0);
    Vertex l1 = Vertex(7.0f, 0.0, 4.99, 1.0);
    Vertex l2 = Vertex(5.0, 2.0f, 4.99, 1.0);
    Triangle lightTriangle = Triangle(l0, l2, l1, scene.getMaterial(lightIndex));
    scene.addLight(lightTriangle, whiteColor);

    Vertex l0Top = Vertex(5.0, -2.0f, 4.995, 1.0);
    Vertex l1Top = Vertex(7.0f, 0.0, 4.995, 1.0);
    Vertex l2Top = Vertex(5.0, 2.0f, 4.995, 1.0);
    Triangle lightTriangleTop = Triangle(l0Top, l1Top, l2Top, scene.getMaterial(lightIndex));
    //scene.addLight(lightTriangleTop, whiteColor);


    Vertex l3 = Vertex(1.0, -3.0f, 4.95, 1.0);
    Vertex l4 = Vertex(3.0f, 0.0, 4.95, 1.0);
    Vertex l5 = Vertex(1.0, 1.0f, 4.95, 1.0);
    Triangle lightTriangle2 = Triangle(l3, l5, l4, scene.getMaterial(lightIndex));
 //   scene.addLight(lightTriangle2, whiteColor);

    Vertex l6 = Vertex(1.0, -3.0f, -4.95f, 1.0);
    Vertex l7 = Vertex(3.0f, 0.0, -4.95f, 1.0);
    Vertex l8 = Vertex(1.0, 1.0f, -4.95f, 1.0);
    Triangle lightTriangle3 = Triangle(l6, l7, l8, scene.getMaterial(lightIndex));
   // scene.addLight(lightTriangle3, whiteColor);

    Vertex l9 = Vertex(5.0, -2.0f, -4.9f, 1.0);
    Vertex l10 = Vertex(7.0f, 0.0, -4.9f, 1.0);
    Vertex l11 = Vertex(5.0, 2.0f, -4.9f, 1.0);
    Triangle lightTriangle4 = Triangle(l9, l11, l10, scene.getMaterial(lightIndex));
    //scene.addLight(lightTriangle4, whiteColor);

    return scene;
}

