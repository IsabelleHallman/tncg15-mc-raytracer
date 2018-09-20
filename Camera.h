//
// Created by Isabelle Hallman on 2018-09-20.
//

#ifndef TNCG15_MC_RAYTRACER_CAMERA_H
#define TNCG15_MC_RAYTRACER_CAMERA_H

#import "Geometry.h"

class Camera {
public:
    Camera(glm::vec3 positionIn = glm::vec3(0.0, 0.0, 0.0),
           Direction n = Direction(1.0, 0.0, 0.0),
           float scale = 2,
           size_t size = 800)
            : position(positionIn), width(size), height(size), pixelSize(scale / size),
              frontEye(Vertex(-1.0f, 0.0, 0.0, 1.0)), backEye(Vertex(-2.0f, 0.0, 0.0, 1.0)) {
        frontEyeActive = true;
    }

    // TODO implement
    // Launches a ray through each pixel one at a time
    void render() {}

    // TODO implement
    // Convert sensor data into to 2D array of rgb vectors with integer values 0 < pixel < 255
    void createImage() {}

private:
    struct Pixel {
        ColorDbl color;
        Ray rays [1]; //Will these always be references?
    };

    const size_t width, height;
    bool frontEyeActive;
    Vertex frontEye, backEye;
    Pixel sensor [width][height];
    float pixelSize;
    glm::vec3 position;
};

#endif //TNCG15_MC_RAYTRACER_CAMERA_H
