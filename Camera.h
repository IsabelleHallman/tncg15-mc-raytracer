//
// Created by Isabelle Hallman on 2018-09-20.
//

#ifndef TNCG15_MC_RAYTRACER_CAMERA_H
#define TNCG15_MC_RAYTRACER_CAMERA_H

#include <cstdio>
#include <iostream>
#include "RayTrace.h"

struct Pixel {
    Pixel() : color(ColorDbl()), pixelMiddlePosition(Vertex()), ray(Ray()) { }

    Pixel(ColorDbl colorIn, Vertex pixelMiddlePositionIn, Ray rayIn)
            : color(colorIn), pixelMiddlePosition(pixelMiddlePositionIn), ray(rayIn) { }

    ColorDbl color;
    Vertex pixelMiddlePosition;
    Ray ray; //TODO: implement possible to have more rays per pixel
};

template <typename T> class Array2D {
public:
    size_t width, height;
    T * array;

    Array2D(size_t widthIn, size_t heightIn) : width(widthIn), height(heightIn) {
        array = new T[width * height];
    }

    T* get(size_t x, size_t y) {
        return &(array[getPosition(x, y)]);
    }

    void set(T value, size_t x, size_t y) {
        array[getPosition(x, y)] = value;
    }

private:
    size_t getPosition(size_t x, size_t y) {
        return x + width * y;
    }
};

class Camera {
public:
    Camera(Scene* sceneIn, glm::vec3 positionIn = glm::vec3(0.0, 0.0, 0.0), float scale = 2, int size = 800)
            : scene(sceneIn), position(positionIn), pixelSize(scale / size),
              frontEye(Vertex(-1.0f, 0.0, 0.0, 1.0)), backEye(Vertex(-2.0f, 0.0, 0.0, 1.0)), frontEyeActive(true),
              topLeft(position + glm::vec3(0.0, - scale / 2, scale / 2)),
              topRight(position + glm::vec3(0.0, scale / 2, scale / 2)),
              bottomLeft(position + glm::vec3(0.0, - scale / 2, - scale / 2)),
              bottomRight(position + glm::vec3(0.0, scale / 2, - scale / 2)),
              sensor(Array2D<Pixel>(width, height)) { }

    // Launches a ray through each pixel one at a time
    void render() {
        RayTrace rayTrace = RayTrace(scene);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Vertex thisPixelMiddlePosition = Vertex(topLeft.position + glm::vec3(0.0,
                                                                                     pixelSize / 2 + x * pixelSize,
                                                                                     - (pixelSize / 2 + y * pixelSize)));
                Vertex* activeEye = frontEyeActive ? &frontEye : &backEye;
                Ray thisRay = Ray(&thisPixelMiddlePosition, Direction(thisPixelMiddlePosition.position - activeEye->position));
                //scene->findIntersectedTriangle(thisRay);
                //ColorDbl thisColor = thisRay.intersection->material->color;
                ColorDbl thisColor = rayTrace.trace(thisRay);
                Pixel thisPixel = Pixel(thisColor, thisPixelMiddlePosition, thisRay);
                sensor.set(thisPixel, x, y);
            }
        }
    }

    // Convert sensor data into to 2D array of rgb vectors with integer values 0 < pixel < 255
    void createImage() {
        FILE *f = fopen("cameraOut.ppm", "wb");
        fprintf(f, "P6\n%i %i 255\n", width, height);

        double iMax = findIMax();
        double normFactor = 255.99 / iMax;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Pixel* thisPixel = sensor.get(x, y);
                fputc((int) (thisPixel->color.r * normFactor), f);
                fputc((int) (thisPixel->color.g * normFactor), f);
                fputc((int) (thisPixel->color.b * normFactor), f);
            }
        }
        fclose(f);
    }

    double findIMax() {
        //TODO: More effective way to do this?
        double iMax = 0.0;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Pixel* thisPixel = sensor.get(x, y);
                if (thisPixel->color.r > iMax) iMax = thisPixel->color.r;
                if (thisPixel->color.g > iMax) iMax = thisPixel->color.g;
                if (thisPixel->color.b > iMax) iMax = thisPixel->color.b;
            }
        }

        return iMax;
    }

private:
    Scene* scene;
    glm::vec3 position;
    float pixelSize;
    Vertex frontEye, backEye;
    bool frontEyeActive;
    Vertex topLeft, topRight, bottomLeft, bottomRight;
    const int width = 800;
    const int height = 800;
    Array2D<Pixel> sensor;
};

#endif //TNCG15_MC_RAYTRACER_CAMERA_H
