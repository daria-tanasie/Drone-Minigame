#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"


namespace implemented
{
    class Camera_H
    {
    public:
        Camera_H()
        {
            position = glm::vec3(0, 2, 5);
            forward = glm::vec3(0, 0, -1);
            up = glm::vec3(0, 1, 0);
            right = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
        }

        Camera_H(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            Set(position, center, up);
        }

        ~Camera_H()
        { }

        // Update Camera_H
        void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            this->position = position;
            forward = glm::normalize(center - position);
            right = glm::cross(forward, up);
            this->up = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void MoveRight(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
            position += dir * distance;
        }


        void TranslateForward(float distance)
        {
            position = position + glm::normalize(forward) * distance;
        }

        void TranslateUpward(float distance)
        {
            position = position + glm::normalize(up) * distance;
        }

        void TranslateRight(float distance)
        {
            position = position + glm::normalize(right) * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1);
            forward = glm::normalize(glm::vec3(newVector));

            newVector = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(up, 1);
            up = glm::normalize(glm::vec3(newVector));

        }

        void RotateFirstPerson_OY(float angle)
        {
            glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
            forward = glm::normalize(glm::vec3(newVector));

            newVector = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(up, 1);
            up = glm::normalize(glm::vec3(newVector));

            newVector = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
            right = glm::normalize(glm::vec3(newVector));
        }

        void RotateFirstPerson_OZ(float angle)
        {
            glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(forward, 1);
            forward = glm::normalize(glm::vec3(newVector));

            newVector = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(up, 1);
            up = glm::normalize(glm::vec3(newVector));
        }

        void RotateThirdPerson_OX(float angle)
        {
            TranslateRight(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateRight(-distanceToTarget);
        }

        void RotateThirdPerson_OY(float angle)
        {
            TranslateUpward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateUpward(-distanceToTarget);
        }

        void RotateThirdPerson_OZ(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

    public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    };
}   // namespace implemented
