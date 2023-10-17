#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../dd11/transformations.h"

namespace dd11 {

	struct Camera {
		ew::Vec3 position = ew::Vec3(0, 0, 5); //Camera body position
		ew::Vec3 target = ew::Vec3(0, 0, 0); //Position to look at
		float fov = 60; //Vertical field of view in degrees
		float aspectRatio; //Screen width / Screen height
		float nearPlane = 0.1; //Near plane distance (+Z)
		float farPlane = 100; //Far plane distance (+Z)
		bool orthographic = false; //Perspective or orthographic?
		float orthoSize = 6; //Height of orthographic frustum
		ew::Mat4 ViewMatrix() { //World->View
			return dd11::LookAt(position, target, ew::Vec3(0, 1, 0));
		}
		ew::Mat4 ProjectionMatrix() { //View->Clip
			if (orthographic) {
				return dd11::Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
			}
			else {
				return dd11::Perspective(fov * dd11::CONVERT_TO_RADIANS, aspectRatio, nearPlane, farPlane);
			}
		}
	};
}
