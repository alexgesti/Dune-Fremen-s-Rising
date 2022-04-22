#pragma once
#include "Component.h"

#include "Bone.h"
#include <map>

#include <queue>

class Animation;
struct HierarchyData;

struct AnimState
{
	std::string state;
	std::shared_ptr<Animation> anim;
	bool loop;
	bool hasFinished = false;
};

class AnimationComponent : public Component
{
public:
	AnimationComponent(GameObject* own);
	AnimationComponent(AnimationComponent* animation);
	~AnimationComponent();

	void OnEditor() override;
	void AnimationInfo();
	bool Update(float dt) override;

	void CalculateBoneTransform(HierarchyData& data, float4x4 parentTransform);
	void CalculateBoneTransform(std::vector<Bone>& bones, float4x4 parentTransform);

	float4x4 InterpolateWithoutBones(float4x4& transform, float4x4& lastTransform);
	float4x4 InterpolateWithOneBone(float4x4& transform, Bone& bone);
	float4x4 InterpolateWithOneBone(Bone& bone, float4x4& transform);

	bool OnLoad(JsonParsing& node) override;
	bool OnSave(JsonParsing& node, JSON_Array* array) override;

	void SetAnimation(std::shared_ptr<Resource> a);
	void Play(std::string state);

	void GetAnimations();
	bool HasFinished() { return currAnim->hasFinished; }

	inline std::vector<float4x4> GetFinalBoneMatrices() { return finalBoneMatrices; };

	const float4x4& UpdateBone(Bone* bone, Bone* lastBone, float4x4& parentTransform);

	std::map<std::string, BoneInfo>::const_iterator GetBoneInfo(std::string name, const std::map<std::string, BoneInfo>& map);
public:
	bool showAnimMenu = false;
	bool playing;

	bool interpolating;
	float interpolatingVel;

	float lastCurrentTime;
	float currentTime;
	float loopTime;
	float deltaTime;

	AnimState* lastAnim;
	AnimState* currAnim;

	std::map<std::string, BoneInfo> boneInfoMap;

	std::vector<float4x4> finalBoneMatrices;
	std::vector<AnimState> animations;
	std::queue<AnimState*> animQueue;
};