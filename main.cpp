//
// Created by Isabelle Hallman on 2018-09-17.
//

#include "Scene.h"
#include <list>
#include <iostream>

int main() {
    Vertex v1 = Vertex(.0, .0, .0, .0);
    Vertex v2 = Vertex(.1, .1, .1, .1);
    Vertex v3 = Vertex(.2, .2, .2, .2);
    ColorDbl color = ColorDbl(1., 1., 1.);
    Ray myRay = Ray(&v1, &v2, &color);


    std::list<Triangle> triangles = {Triangle(v1, v2, v3, color)};
    Scene myScene = Scene(triangles);

    Triangle& intersectedTriangle = myScene.findIntersectedTriangle(myRay);

    std::cout << intersectedTriangle.v2.x << std::endl;

    return 0;
}