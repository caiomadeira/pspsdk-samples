#include "camera.h"

const float CAMERA_DISTANCE = 10.0f;
const float CAMERA_HEIGHT = 2.0f;

void InitGameCamera(Camera3D* camera) {
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera->fovy = 45.0f;
    camera->projection = CAMERA_PERSPECTIVE; 
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera->target = (Vector3){ 0.0f, 0.0f, 0.0f };
}

void UpdateGameCamera(Camera3D* camera, Vector3 playerPos, float playerRot) {
    camera->target = (Vector3){ playerPos.x, playerPos.y + 1.5f, playerPos.z };
    camera->position.x = playerPos.x - (sinf(playerRot * DEG2RAD) * CAMERA_DISTANCE_BEHIND);
    camera->position.z = playerPos.z - (cosf(playerRot * DEG2RAD) * CAMERA_DISTANCE_BEHIND);
    camera->position.y = playerPos.y + CAMERA_HEIGHT_ABOVE;
}