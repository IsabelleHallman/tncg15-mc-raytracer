//
// Created by Isabelle Hallman on 2018-09-20.
//

#ifndef TNCG15_MC_RAYTRACER_CAMERA_H
#define TNCG15_MC_RAYTRACER_CAMERA_H

#include <cstdio>
#include <iostream>
#include "Scene.h"
struct Pixel {
    // TODO: Possible to fix this ugly constructor?
    Pixel() : color(ColorDbl()), pixelMiddlePosition(Vertex()), ray(Ray()) { std::cout << "kom hit i alla fall" << std::endl;}

    ColorDbl color;
    Ray ray; //TODO: implement possible to have more rays per pixel
    Vertex pixelMiddlePosition;
};

class Camera {
public:
    Camera(Scene* sceneIn, glm::vec3 positionIn = glm::vec3(0.0, 0.0, 0.0),
           Direction n = Direction(1.0, 0.0, 0.0),
           float scale = 2,
           int size = 800)
            : scene(sceneIn), position(positionIn), pixelSize(scale / size),
              frontEye(Vertex(-1.0f, 0.0, 0.0, 1.0)), backEye(Vertex(-2.0f, 0.0, 0.0, 1.0)), frontEyeActive(true),
              topLeft(position + glm::vec3(0.0, - scale / 2, scale / 2)),
              topRight(position + glm::vec3(0.0, scale / 2, scale / 2)),
              bottomLeft(position + glm::vec3(0.0, - scale / 2, - scale / 2)),
              bottomRight(position + glm::vec3(0.0, scale / 2, - scale / 2)) { }

    // Launches a ray through each pixel one at a time
    void render() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                sensor[x][y].pixelMiddlePosition = Vertex(topLeft.position + glm::vec3(0.0,
                                                                              pixelSize / 2 + x * pixelSize,
                                                                              pixelSize / 2 + y * pixelSize));
                Vertex* activeEye = frontEyeActive ? &frontEye : &backEye;
                Camera.sensor[x][y].ray = Ray(activeEye, &sensor[x][y].pixelMiddlePosition);
                sensor[x][y].color = scene->findIntersectedTriangle(sensor[x][y].ray).color;
            }
        }
    }

    // Convert sensor data into to 2D array of rgb vectors with integer values 0 < pixel < 255
    void createImage() {
        FILE *f = fopen("cameraOut.ppm", "wb");
        fprintf(f, "P6\n%i %i 255\n", width, height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // TODO: truncate colors (divide by maxI for entire image)
                 fputc((int) sensor[x][y].color.r * 255, f);
                 fputc((int) sensor[x][y].color.g * 255, f);
                 fputc((int) sensor[x][y].color.b * 255, f);
            }
        }
        fclose(f);
    }

private:


    //TODO: Fix problem where sensor needs to be allocated dynamically
    static const int width = 800;
    static const int height = 800;
    bool frontEyeActive;
    Vertex frontEye, backEye;
    static Pixel sensor[width][height];
    float pixelSize;
    glm::vec3 position;
    Scene* scene;
    Vertex topLeft, topRight, bottomLeft, bottomRight;
};

#endif //TNCG15_MC_RAYTRACER_CAMERA_H
