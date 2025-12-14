#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

#define  CAMERA_DISTANCE_BEHIND 10.0f
#define  CAMERA_HEIGHT_ABOVE 2.0f

void InitGameCamera(Camera3D* camera);
void UpdateGameCamera(Camera3D* camera, Vector3 playerPos, float playerRot);

#endif