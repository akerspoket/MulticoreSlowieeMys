#include "CameraManager.h"



CameraManager::CameraManager()
{
	XMStoreFloat4x4(&mWorldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&mViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)), XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMLoadFloat3(&XMFLOAT3(0, 1, 0)))));
	XMStoreFloat4x4(&mProjectionMatrix, XMMatrixTranspose(XMMatrixPerspectiveFovLH(45.0f, SCREEN_HEIGHT / SCREEN_WIDTH, 0.1f, 100)));
	mCamUp = XMFLOAT3(0, 1, 0);
	mCamTarget = XMFLOAT3(0, 0, 1);
	mCamPos = XMFLOAT3(0, 0, 0);
}


CameraManager::~CameraManager()
{
}

void CameraManager::UpdateCamera()
{
	XMStoreFloat4x4(&mCamRotationMatrix,XMMatrixRotationRollPitchYaw(mCamPitch, mCamYaw, 0));
	XMStoreFloat3(&mCamTarget, XMVector3TransformCoord(XMLoadFloat3( &mDefaultForward), XMLoadFloat4x4(&mCamRotationMatrix)));
	XMStoreFloat3(&mCamTarget, XMVector3Normalize(XMLoadFloat3( &mCamTarget)));

	XMFLOAT4X4  tRotateMatrix;
	XMStoreFloat4x4(&tRotateMatrix, XMMatrixRotationY(mCamYaw));
    
	XMStoreFloat3(&mCamRight, XMVector3TransformCoord(XMLoadFloat3(&mDefaultRight), XMLoadFloat4x4(&tRotateMatrix)));

	XMStoreFloat3(&mCamUp, XMVector3TransformCoord(XMLoadFloat3(&mCamUp), XMLoadFloat4x4(&tRotateMatrix)));
	XMStoreFloat3(&mCamForward, XMVector3TransformCoord( XMLoadFloat3(&mDefaultForward), XMLoadFloat4x4(&tRotateMatrix)));

	XMVECTOR tempCam;
	tempCam = XMLoadFloat3(&mCamPos);

	tempCam += mMoveLeftRight * XMLoadFloat3(&mCamRight);
	tempCam += mMoveForwardBackward * XMLoadFloat3(&mCamForward);

	XMStoreFloat3(&mCamPos, tempCam);

	mMoveLeftRight = 0.0f;
	mMoveForwardBackward = 0.0f;
	
	XMStoreFloat3(&mCamTarget, XMLoadFloat3(&mCamPos) + XMLoadFloat3(&mCamTarget));

	XMStoreFloat4x4(&mViewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&mCamPos),XMLoadFloat3(&mCamTarget), XMLoadFloat3(&mCamUp)));
	XMStoreFloat3(&mCamRight, XMVector3Normalize(XMLoadFloat3(&mCamRight)));

}
void CameraManager::HandleUserCMD(float pDT,UserCMD pUserCMD)
{
	float speed = 0.1f * pDT;
	if (pUserCMD.a)
	{
		mMoveLeftRight -= speed;
	}
	if (pUserCMD.d)
	{
		mMoveLeftRight += speed;
	}
	if (pUserCMD.s)
	{
		mMoveForwardBackward += speed;
	}
	if (pUserCMD.w)
	{
		mMoveForwardBackward -= speed;
	}
	mCamYaw += pUserCMD.mouseX*0.01f;
	mCamPitch += pUserCMD.mouseY*0.01f;
	
}
XMFLOAT4X4 CameraManager::GetWVP()
{
	XMFLOAT4X4 rWVP;
	XMStoreFloat4x4(&rWVP, XMMatrixTranspose(XMLoadFloat4x4(& mWorldMatrix)*XMLoadFloat4x4(&mViewMatrix)*XMLoadFloat4x4(&mProjectionMatrix)));
	return rWVP;
}
XMFLOAT4X4 CameraManager::GetRotationMatrix()
{
	XMFLOAT4X4 rRot;
	XMStoreFloat4x4(&rRot, XMMatrixTranspose(XMLoadFloat4x4(&mCamRotationMatrix)));
	return rRot;
}
