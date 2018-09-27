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

    ColorDbl red = ColorDbl(1.0, 0.0, 0.0);
    ColorDbl green = ColorDbl(0.0, 1.0, 0.0);
    ColorDbl blue = ColorDbl(0.0, 0.0, 1.0);
    ColorDbl yellow = ColorDbl(1.0, 1.0, 0.0);
    ColorDbl magenta = ColorDbl(1.0, 0.0, 1.0);
    ColorDbl cyan = ColorDbl(0.0, 1.0, 1.0);
    ColorDbl white = ColorDbl(1.0, 1.0, 1.0);


    std::list<Triangle> sceneTriangles = {
            Triangle(sceneVertices.at(0), sceneVertices.at(2), sceneVertices.at(1), red),
            Triangle(sceneVertices.at(2), sceneVertices.at(1), sceneVertices.at(3), red),
            Triangle(sceneVertices.at(3), sceneVertices.at(2), sceneVertices.at(4), green),
            Triangle(sceneVertices.at(3), sceneVertices.at(4), sceneVertices.at(5), green),
            Triangle(sceneVertices.at(4), sceneVertices.at(6), sceneVertices.at(5), blue),
            Triangle(sceneVertices.at(5), sceneVertices.at(6), sceneVertices.at(7), blue),
            Triangle(sceneVertices.at(6), sceneVertices.at(8), sceneVertices.at(7), yellow),
            Triangle(sceneVertices.at(7), sceneVertices.at(8), sceneVertices.at(9), yellow),
            Triangle(sceneVertices.at(8), sceneVertices.at(10), sceneVertices.at(9), magenta),
            Triangle(sceneVertices.at(9), sceneVertices.at(10), sceneVertices.at(11), magenta),
            Triangle(sceneVertices.at(10), sceneVertices.at(0), sceneVertices.at(11), cyan),
            Triangle(sceneVertices.at(0), sceneVertices.at(1), sceneVertices.at(11), cyan),
            Triangle(sceneVertices.at(0), sceneVertices.at(10), sceneVertices.at(8), white),
            Triangle(sceneVertices.at(0), sceneVertices.at(8), sceneVertices.at(12), white),
            Triangle(sceneVertices.at(0), sceneVertices.at(12), sceneVertices.at(2), white),
            Triangle(sceneVertices.at(2), sceneVertices.at(12), sceneVertices.at(6), white),
            Triangle(sceneVertices.at(12), sceneVertices.at(8), sceneVertices.at(6), white),
            Triangle(sceneVertices.at(2), sceneVertices.at(6), sceneVertices.at(4), white),
            Triangle(sceneVertices.at(11), sceneVertices.at(1), sceneVertices.at(9), white),
            Triangle(sceneVertices.at(1), sceneVertices.at(13), sceneVertices.at(9), white),
            Triangle(sceneVertices.at(1), sceneVertices.at(3), sceneVertices.at(13), white),
            Triangle(sceneVertices.at(13), sceneVertices.at(3), sceneVertices.at(7), white),
            Triangle(sceneVertices.at(13), sceneVertices.at(7), sceneVertices.at(9), white),
            Triangle(sceneVertices.at(3), sceneVertices.at(5), sceneVertices.at(7), white)
    };
    Scene scene = Scene(sceneTriangles);

    Vertex tetraPosition = Vertex(glm::vec3(8.0, 3.0, -4.0));
    ColorDbl tetraColor = ColorDbl(.5, 1., 1.);
    scene.addTetrahedron(tetraPosition, tetraColor);

    return scene;
}

int main() {
    Vertex v1 = Vertex(.0, .0, .0, .0);
    Vertex v2 = Vertex(.0, .0, .1, .1);
    Vertex v3 = Vertex(.1, .0, .0, .2);
    ColorDbl color = ColorDbl(1., 1., 1.);
    Ray myRay = Ray(&v1, &v2);

    Scene myScene = generateTestScene();

    Triangle& intersectedTriangle = myScene.findIntersectedTriangle(myRay);

    Camera camera = Camera(&myScene);
    camera.render();
    camera.createImage();
    return 0;
}

