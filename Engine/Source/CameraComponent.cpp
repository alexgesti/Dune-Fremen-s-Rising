#include "Application.h"
#include "Globals.h"

#include "CameraComponent.h"
#include "GameObject.h"

#include "ModuleScene.h"
#include "ModuleInput.h"

#include "glew/include/GL/glew.h"

#include "IconsFontAwesome5.h"

#include "Profiling.h"

#include <stdlib.h>

CameraComponent::CameraComponent(GameObject* own, TransformComponent* trans) : horizontalFov(DegToRad(90.0f)), verticalFov(0.0f), nearPlane(1.0f), farPlane(100.0f), transform(trans), currentRotation(0,0,0,1), currentScreenHeight(SCREEN_HEIGHT), currentScreenWidth(SCREEN_WIDTH), vbo(nullptr), ebo(nullptr)
{
	type = ComponentType::CAMERA;
	owner = own;
	camera.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	camera.SetViewPlaneDistances(nearPlane, farPlane);
	CalculateVerticalFov(horizontalFov, currentScreenWidth, currentScreenHeight);
	camera.SetPerspective(horizontalFov, verticalFov);
	camera.SetFrame(float3(0.0f,0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), float3(0.0f, 1.0f, 0.0f));

	srand(time(NULL));

	CompileBuffers();
}

CameraComponent::~CameraComponent()
{
	RELEASE(vbo);
	RELEASE(ebo);
}

void CameraComponent::OnEditor()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(ICON_FA_CAMERA" Camera"))
	{
		ImGui::Text("Field of view");
		ImGui::SameLine();
		static float horizontalFovEditor = RadToDeg(horizontalFov);
		if (ImGui::DragFloat("", &horizontalFovEditor, 1.0f, 1.0f, 179.0f))
		{
			horizontalFov = DegToRad(horizontalFovEditor);
			UpdateFov();
			CompileBuffers();
		}

		ImGui::Text("Clipping planes");

		ImGui::Text("Near");
		ImGui::SameLine();
		ImGui::PushID("NearPlane");
		if (ImGui::DragFloat("", &nearPlane, 0.5f, 0.1f)) SetPlanes();
		ImGui::PopID();

		ImGui::PushID("farPlane");
		ImGui::Text("Far");
		ImGui::SameLine();
		if (ImGui::DragFloat("", &farPlane, 0.5f, 0.1f)) SetPlanes();
		ImGui::PopID();

		ImGui::PushID("farPlane");
		ImGui::Text("Far");
		ImGui::SameLine();
		if (ImGui::DragFloat("", &farPlane, 0.5f, 0.1f)) SetPlanes();
		ImGui::PopID();

		ImGui::Text("- - - - - - - - -");

		if (ImGui::Checkbox("freeMovement", &freeMovement)) {}

		if (ImGui::Checkbox("followTarget", &followTarget)) {}
		ImGui::SameLine();
		ImGui::Button(target != nullptr ? target->GetName() : "none");
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* go = ImGui::AcceptDragDropPayload("HierarchyItem");
			if (go)
			{
				uint uuid = *(const uint*)(go->Data);
				target = app->scene->GetGoByUuid(uuid);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::Checkbox("multifocusOnClick", &multifocusOnClick)) {}
		//ImGui::SameLine(); select game object

		ImGui::Text("s_lerp");
		//ImGui::SameLine(); select mode none/lerp/slerp

		if (ImGui::DragFloat("verticalAngle", &verticalAngle, 0.01f, 0.0f/*, 90.0f*/)) {}

		if (ImGui::Checkbox("rotateAround", &rotateAround)) {}
		if (ImGui::DragFloat("rotationSpeed", &rotationSpeed, 0.01f, 0.0f/*, 90.0f*/)) {}


		ImGui::Text("- - - - - - - - -");

		if (ImGui::DragFloat("shakeStrength", &shakeStrength, 0.1f, 0.0f/*, 90.0f*/)) {}
		if (ImGui::DragFloat("shakeDuration", &shakeDuration, 0.1f, 0.0f/*, 90.0f*/)) {}
		if (ImGui::Checkbox("smoothShake", &smoothShake)) {}
		if (ImGui::Button("Shake")) {
			RequestShake(shakeStrength, shakeDuration);
		}
	}
	ImGui::PopID();
}

bool CameraComponent::Update(float dt)
{
	
	camera.SetPos(transform->GetPosition());

	if (shake)
	{
		Shake(dt);
	}
	
	if (!CompareRotations(currentRotation, transform->GetRotation()))
	{
		currentRotation = transform->GetRotation();
	
		float3 newUp = float3::unitY;
		float3 newFront = float3::unitZ;
		
		newUp = transform->GetRotation() * newUp;
		
		newUp.Normalize();
		newFront = transform->GetRotation() * newFront;
		newFront.Normalize();

		float3::Orthonormalize(newUp, newFront);
		
		camera.SetUp(newUp);
		camera.SetFront(newFront);
	}

	if (followTarget && target)
	{
		float3 targetPos = target->GetComponent<TransformComponent>()->GetPosition();
		float3 newPos = targetPos;
		float offsetZ = 20.0f;
		float offsetX = 20.0f;
		newPos.z += 20;
		newPos.y += 20;

		// This is from LookAt function from ModuleCamera.h
		float3 directionFrustum = targetPos - camera.Pos();
		directionFrustum.Normalize();

		float3x3 lookAt = float3x3::LookAt(camera.Front(), directionFrustum, camera.Up(), float3(0.0f, 1.0f, 0.0f));
		camera.SetFront(lookAt.MulDir(camera.Front()).Normalized());
		camera.SetUp(lookAt.MulDir(camera.Up()).Normalized());

		if (rotateAround)
		{
			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT) horizontalAngle -= rotationSpeed;
			if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT) horizontalAngle += rotationSpeed;

			// Calculate rotation position
			float cosinus = cos(horizontalAngle);
			float sinus = sin(horizontalAngle);
			float TestPosZ = newPos.z * cosinus - newPos.x * sinus;
			float TestPosX = newPos.x * cosinus + newPos.z * sinus;
			// Add offset with angle 0
			newPos.z += TestPosZ - newPos.z;
			newPos.x += TestPosX - newPos.x;
		}

		transform->SetPosition(newPos);
		transform->SetRotation(lookAt.ToQuat());
	}

	matrixProjectionFrustum = camera.ComputeProjectionMatrix();
	matrixViewFrustum = camera.ComputeViewMatrix();

	return true;
}

void CameraComponent::Draw()
{
	glPushMatrix();

	glMultMatrixf(transform->GetGlobalTransform().Transposed().ptr());
	glEnableClientState(GL_VERTEX_ARRAY);
	vbo->Bind();
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	ebo->Bind();
	glLineWidth(2.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glDrawElements(GL_LINES, ebo->GetSize(), GL_UNSIGNED_INT, NULL);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	vbo->Unbind();
	ebo->Unbind();
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

void CameraComponent::SetPlanes()
{
	camera.SetViewPlaneDistances(nearPlane, farPlane);
	CompileBuffers();
}

void CameraComponent::CalculateVerticalFov(float horizontalFovRadians, float width, float height)
{
	verticalFov = 2 * Atan((Tan(horizontalFovRadians / 2)) * (height / width));
	camera.SetVerticalFovAndAspectRatio(verticalFov, (width / height));
	currentScreenHeight = height;
	currentScreenWidth = width;
}

void CameraComponent::UpdateFovAndScreen(float width, float height)
{
	verticalFov = 2 * Atan((Tan(horizontalFov / 2)) * (height / width));
	camera.SetVerticalFovAndAspectRatio(verticalFov, (width / height));
	currentScreenHeight = height;
	currentScreenWidth = width;
}

void CameraComponent::UpdateFov()
{
	verticalFov = 2 * Atan((Tan(horizontalFov / 2)) * (currentScreenHeight / currentScreenWidth));
	camera.SetVerticalFovAndAspectRatio(verticalFov, (currentScreenWidth / currentScreenHeight));
}

void CameraComponent::CompileBuffers()
{
	// Configure buffers
	float3 corners[8];
	camera.GetCornerPoints(corners);
	unsigned int indices[24] =
	{
		0,1,
		1,3,
		3,2,
		2,0,

		1,5,
		4,6,
		7,3,

		6,7,
		6,2,

		7,5,
		4,5,

		4,0
	};

	if (vbo)
	{
		vbo->Unbind();
		RELEASE(vbo);
	}
	ebo = new IndexBuffer(indices, 24);
	vbo = new VertexBuffer(corners, sizeof(float3) * 8);
	ebo->Unbind();
	vbo->Unbind();
}

bool CameraComponent::CompareRotations(Quat& quat1, Quat& quat2)
{
	if (quat1.x == quat2.x && quat1.y == quat2.y && quat1.z == quat2.z && quat1.w == quat2.w) return true;

	return false;
}

bool CameraComponent::OnLoad(JsonParsing& node)
{
	nearPlane = node.GetJsonNumber("Near Plane");
	farPlane = node.GetJsonNumber("Far Plane");
	verticalFov = node.GetJsonNumber("Vertical Fov");
	horizontalFov = node.GetJsonNumber("Horizontal Fov");
	camera.SetPos(node.GetJson3Number(node, "Camera Pos"));

	return true;
}

bool CameraComponent::OnSave(JsonParsing& node, JSON_Array* array)
{
	JsonParsing file = JsonParsing();

	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Near Plane", nearPlane);
	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Far Plane", farPlane);
	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Vertical Fov", verticalFov);
	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Horizontal Fov", horizontalFov);
	file.SetNewJson3Number(file, "Camera Pos", camera.Pos());
	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Type", (int)type);

	node.SetValueToArray(array, file.GetRootValue());

	return true;
}


int CameraComponent::ContainsAaBox(const AABB& boundingBox)
{
	if (boundingBox.IsFinite())
	{
		if (camera.Contains(boundingBox))
		{
			return 1;
		}
		else if (camera.Intersects(boundingBox))
		{
			return 2;
		}
		return 0;
	}
	return -1;
}

void CameraComponent::RequestShake(float strength, float duration)
{
	shakeStrength = strength;
	shakeDuration = duration;
	if (smoothShake) currentStrength = 0;
	else currentStrength = strength;
	shake = true;
	originalPos = transform->GetPosition();
}

void CameraComponent::Shake(float dt)
{
	// Exponential
	if (smoothShake)
	{
		if (elapsedTime < shakeDuration / 2)
		{
			if (currentStrength < shakeStrength)
			{
				currentStrength = shakeStrength * elapsedTime * elapsedTime;
			}
			else currentStrength = shakeStrength;
		}
		else if (currentStrength > 0 && elapsedTime < shakeDuration)
		{
			currentStrength = shakeStrength * (shakeDuration - elapsedTime) * (shakeDuration - elapsedTime);
		}
		else currentStrength = 0;
	}

	if (elapsedTime < shakeDuration)
	{
		float x = -currentStrength + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (currentStrength - -currentStrength)));
		float y = -currentStrength + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (currentStrength - -currentStrength)));
		float3 lastPos = transform->GetPosition();
		lastPos.x += x;
		lastPos.y += y;
		camera.SetPos(lastPos);
		elapsedTime += dt;
	}
	else
	{
		shake = false;
		elapsedTime = 0.0f;
		camera.SetPos(originalPos);
	}
}