#pragma once
#include <DirectXMath.h>
#include "Globals.h"
using namespace DirectX;
class CameraManager
{
public:
	CameraManager();
	~CameraManager();
	void UpdateCamera();
	void HandleUserCMD(float pDT,UserCMD pUserCmd);
	XMFLOAT4X4 GetWVP();
	XMFLOAT4X4 GetRotationMatrix();
private:
	XMFLOAT3 mDefaultForward = XMFLOAT3( 0.0f, 0.0f, 1.0f);
	XMFLOAT3 mDefaultRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 mCamForward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 mCamRight= XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 mCamTarget = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 mCamPos;
	XMFLOAT3 mCamUp;
	XMFLOAT4X4 mCamRotationMatrix;
	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjectionMatrix;

	float mCamYaw = 0.0f;
	float mCamPitch = 0.0f;

	float mMoveLeftRight = 0.0f;
	float mMoveForwardBackward = 0.0f;
};
