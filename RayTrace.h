//
// Created by rebec on 2018-10-10.
//

#ifndef TNCG15_MC_RAYTRACER_RAYTRACE_H
#define TNCG15_MC_RAYTRACER_RAYTRACE_H

class RayTrace
{
    void createRayTree();

    void trace();

    void getReflectedRay();

    void getRefractedRay();

    void sendShadowRays();

};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
