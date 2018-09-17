//
// Created by Isabelle Hallman on 2018-09-17.
//

#ifndef PROJECT_VERTEX_H
#define PROJECT_VERTEX_H


class Vertex {

public:
    Vertex(float xIn, float yIn, float zIn, float wIn)
            : x(xIn), y(yIn), z(zIn), w(wIn) { }

private:
    float x, y, z, w;
};


#endif //PROJECT_VERTEX_H
