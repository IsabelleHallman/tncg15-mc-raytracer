//
// Created by rebec on 2018-10-10.
//

#ifndef TNCG15_MC_RAYTRACER_RAYTRACE_H
#define TNCG15_MC_RAYTRACER_RAYTRACE_H
#include "Scene.h"
#include "glm/gtx/vector_angle.hpp"

class RayTrace {
public:
    RayTrace(Scene* sceneIn) : scene(sceneIn){ }

    ColorDbl trace(Ray &ray){
        Node root = createRayTree(ray);

        glm::vec3 colorContrib = calculateLight(&root);
        ColorDbl pixelColor = ColorDbl(colorContrib);

        destroyNodeTree(&root);

        return pixelColor;
    }

private:
    struct Node {
        Node(Node* parentIn, Ray rayIn, Scene* scene)
                : parent(parentIn), reflected(nullptr), refracted(nullptr), ray(rayIn) {
            scene->findIntersectedTriangle(ray);
        }

        void calculateRadianceDistribution(float n1, float n2) {
            float R_0 = pow((n1 - n2)/(n1 + n2), 2);
            reflectionCoefficient = R_0 +
                    (1 - R_0) * pow(1 - glm::dot(ray.intersection->normal.vector, -1.f * ray.direction.vector), 5);
        }

        Node* parent;
        Node* reflected;
        Node* refracted;
        float reflectionCoefficient = 1.0f;
        Ray ray;
    };

    Scene* scene;
    const int MAX_DEPTH = 10;

    Node createRayTree(Ray &ray) {
        Node root = Node(nullptr, ray, scene);
        Node* next = &root;

        createNewRayNodes(&root, 0);

        return root;
    }

    void createNewRayNodes(Node* current_node, int depth){
        // Russian Roulette
        float random = std::rand()/RAND_MAX;
        float terminationProbability = (depth != 0) ? 0.2f : 0.0f;
        if(random < terminationProbability)
            return;

        // We terminate if we hit a diffuse object or reach maximum depth as well
        if(current_node->ray.intersection->material->type == LAMBERTIAN
           || current_node->ray.intersection->material->type == OREN_NAYAR
           || depth >= MAX_DEPTH)
            return;

        // Create a new reflected ray
        current_node->reflected = new Node(current_node, getReflectedRay(current_node->ray), scene);
        createNewRayNodes(current_node->reflected, depth + 1);


        // Create a new refracted ray if the material is transparent
        if(current_node->ray.intersection->material->type == TRANSPARENT){
            float n1 = 0.f, n2 = 0.f;
            current_node->refracted = new Node(current_node, getRefractedRay(current_node->ray, n1, n2), scene);
            current_node->calculateRadianceDistribution(n1, n2);
            createNewRayNodes(current_node->refracted, depth+1);
        }
    }

    void destroyNodeTree(Node* root) {
        destroyNode(root->reflected);
        destroyNode(root->refracted);
    }

    void destroyNode(Node* node) {
        if (node) {
            destroyNode(node->reflected);
            destroyNode(node->refracted);
            delete node;
        }
    }

    glm::vec3 calculateLight(Node* currentNode) {
        glm::vec3 reflectedLight = glm::vec3(0.f), refractedLight = glm::vec3(0.f), currentLight = glm::vec3(0.f);

        if(currentNode->reflectionCoefficient > 1.f)
            std::cout << currentNode->reflectionCoefficient << std::endl;

        if(currentNode->reflected != nullptr)
            reflectedLight = currentNode->reflectionCoefficient * calculateLight(currentNode->reflected);
        if(currentNode->refracted != nullptr)
            refractedLight = (1-currentNode->reflectionCoefficient) * calculateLight(currentNode->refracted);

        if (currentNode->ray.intersection->material->type == LIGHT) {
            ColorDbl lightColor = currentNode->ray.intersection->material->color;
            return glm::vec3(lightColor.r, lightColor.g, lightColor.b);
        }
        
        currentLight = reflectedLight + refractedLight;


        // Calculate the contribution from the direct lighting
        if(currentNode->ray.intersection->material->type != PERFECT_REFLECTOR) {
            ColorDbl currentColorDbl = currentNode->ray.intersection->material->color;
            glm::vec3 currentColor = glm::vec3(currentColorDbl.r, currentColorDbl.g, currentColorDbl.b);

            glm::vec3 directLight = calculateDirectLight(&currentNode->ray);

            currentLight += (currentColor * directLight);
        }

        return currentLight;
    }

    glm::vec3 calculateDirectLight(Ray* ray) {
        glm::vec3 allLightsContributions = glm::vec3(0.0);
        int numRays = 5;

        for (auto iterator = scene->lightBegin(); iterator != scene->lightEnd(); ++iterator) {
            glm::vec3 singleLightContribution = glm::vec3(0.0);
            Light* light = &*iterator;
            for (int i = 0; i < numRays; i++)
                singleLightContribution += sendShadowRay(light, ray->intersection, ray);
            singleLightContribution *= (1.0 / numRays) * light->lightTriangle.area;
            allLightsContributions += singleLightContribution;
        }

        return allLightsContributions;
    }

    // TODO: Introduce Monte Carlo scheme by using random directions (slide 209)
    Ray getReflectedRay(Ray &incomingRay) {
        // TODO: Introduce Monte Carlo scheme by using random directions (slide 209) if glossy surface

        glm::vec3 reflectedDir = glm::reflect(incomingRay.direction.vector, incomingRay.intersection->normal.vector);

        glm::vec3 offset = 0.00001f * incomingRay.intersection->normal.vector;
        Vertex reflectedRayOrigin = Vertex(incomingRay.intersection->position.position + offset);

        return Ray(reflectedRayOrigin, Direction(reflectedDir));
    }

    Ray getRefractedRay(Ray &incomingRay, float &n1, float &n2) {
        n1 = 1.0; // AIR
        n2 = incomingRay.intersection->material->refractionIndex;

        glm::vec3 surfaceNormal = incomingRay.intersection->normal.vector;
        glm::vec3 offset = 0.00001f * surfaceNormal;

        // Check if we are inside of the object or outside, swap if we are inside
        float angle = glm::angle(incomingRay.direction.vector, surfaceNormal);
        if(angle < glm::pi<float>()/2.f){
            surfaceNormal*= -1.f;
            offset *= -1.f;
            std::swap(n1,n2);
        }

        // TODO: CHANGE THIS! IF THE ANGLE IS TOO LARGE, THERE SHOULD BE NO REFRACTION RAY
        /*if(n1 > n2){
            float brewsterAngle = glm::asin(n2 / n1);
            float alpha = glm::angle(incomingRay.direction.vector, surfaceNormal);
            if(alpha > brewsterAngle)
                return Ray();
        }*/

        float refractionRatio = n1/n2;
        glm::vec3 refractedRay = refract(incomingRay.direction.vector, surfaceNormal, refractionRatio);
        Vertex refractedRayOrigin = Vertex(incomingRay.intersection->position.position + offset);

        return Ray(refractedRayOrigin, Direction(refractedRay));
    }

    glm::vec3 sendShadowRay(Light* light, Intersection* intersection, Ray* ray) {
        Vertex pointOnLight = light->getRandomPointOnLight();
        glm::vec3 s = pointOnLight.position - intersection->position.position;
        Vertex startPoint = Vertex(intersection->position.position + EPSILON * glm::normalize(s));
        Ray shadowRay = Ray(startPoint, Direction(s));
        if (!scene->findIntersectedTriangle(shadowRay) || shadowRay.intersection->position != pointOnLight)
            return glm::vec3(0.0);

        float d = glm::length(s);
        //s = glm::normalize(s);
        float cosAlpha = - glm::dot(s, light->lightTriangle.normal.vector);
        float cosBeta = glm::dot(s, intersection->normal.vector);

        float g = glm::abs(cosAlpha) * glm::abs(cosBeta) / (d * d);

        glm::vec3 brdfResult = intersection->material->getBRDF(*intersection, shadowRay.direction, ray->direction);

        return g * brdfResult;
    }
};

#endif //TNCG15_MC_RAYTRACER_RAYTRACE_H
