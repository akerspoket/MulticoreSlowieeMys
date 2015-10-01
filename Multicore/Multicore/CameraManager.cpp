#include "CameraManager.h"
#include <iostream>


CameraManager::CameraManager()
{
	XMStoreFloat4x4(&mWorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&mViewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, -25.0f)), XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
	XMStoreFloat4x4(&mProjectionMatrix, XMMatrixPerspectiveFovLH(45.0f, SCREEN_HEIGHT / SCREEN_WIDTH, 0.1f, 100));
	mCamUp = XMFLOAT3(0, 1, 0);
	mCamTarget = XMFLOAT3(0,1 ,0 );
	mCamPos = XMFLOAT3(0, 0, 0);
	mDefaultForward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mDefaultRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCamForward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mCamRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCamTarget = XMFLOAT3(0.0f, 0.0f, 1.0f);
}


CameraManager::~CameraManager()
{
}

void CameraManager::UpdateCamera()
{
	XMStoreFloat4x4(&mCamRotationMatrix,XMMatrixRotationRollPitchYaw(mCamPitch, mCamYaw, 0));
	XMStoreFloat3(&mCamTarget, XMVector3TransformCoord(XMLoadFloat3( &mDefaultForward), XMLoadFloat4x4(&mCamRotationMatrix)));
	XMStoreFloat3(&mCamTarget, XMVector3Normalize(XMLoadFloat3( &mCamTarget)));

	//XMFLOAT4X4  tRotateMatrix;
	//XMStoreFloat4x4(&tRotateMatrix, XMMatrixRotationY(mCamYaw));
 //   
	//XMStoreFloat3(&mCamRight, XMVector3TransformCoord(XMLoadFloat3(&mDefaultRight), XMLoadFloat4x4(&tRotateMatrix)));

	//XMStoreFloat3(&mCamUp, XMVector3TransformCoord(XMLoadFloat3(&mCamUp), XMLoadFloat4x4(&tRotateMatrix)));
	//XMStoreFloat3(&mCamForward, XMVector3TransformCoord( XMLoadFloat3(&mDefaultForward), XMLoadFloat4x4(&tRotateMatrix)));

	XMStoreFloat3(&mCamRight, XMVector3TransformCoord(XMLoadFloat3(&mDefaultRight), XMLoadFloat4x4(&mCamRotationMatrix)));
	XMStoreFloat3(&mCamForward, XMVector3TransformCoord(XMLoadFloat3(&mDefaultForward), XMLoadFloat4x4(&mCamRotationMatrix)));
	XMStoreFloat3(&mCamUp, XMVector3Cross(XMLoadFloat3(&mCamForward), XMLoadFloat3(&mCamRight)));

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
	std::cout << mCamTarget.x << ", " << mCamTarget.y << ", " << mCamTarget.z << std::endl;
}
void CameraManager::HandleUserCMD(float pDT,UserCMD pUserCMD)
{
	float speed = 0.001f * pDT;
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
		mMoveForwardBackward -= speed;
	}
	if (pUserCMD.w)
	{
		mMoveForwardBackward += speed;
	}
	mCamYaw += pUserCMD.mouseX*0.001f;
	mCamPitch += pUserCMD.mouseY*0.001f;
	
}

XMFLOAT4X4 CameraManager::GetRotationMatrix()
{
	XMFLOAT4X4 rRot;
	XMStoreFloat4x4(&rRot, XMMatrixTranspose(XMLoadFloat4x4(&mCamRotationMatrix)));
	return rRot;
}
XMFLOAT4X4 CameraManager::GetWorld()
{
	XMFLOAT4X4 rMatrix;
	XMMATRIX tWorld = XMLoadFloat4x4(&mWorldMatrix);// XMMatrixTranspose(XMLoadFloat4x4(&mWorldMatrix));
	XMStoreFloat4x4(&rMatrix, tWorld);
	return rMatrix;
}
XMFLOAT4X4 CameraManager::GetView()
{
	XMFLOAT4X4 rMatrix;
	XMMATRIX tView = XMLoadFloat4x4(&mViewMatrix);

	tView = XMMatrixInverse(&XMMatrixDeterminant(tView), tView);	
	tView = XMMatrixTranspose(tView);
	XMStoreFloat4x4(&rMatrix, ( tView));
	return rMatrix;
}
XMFLOAT4X4 CameraManager::GetProjection()
{
	XMFLOAT4X4 rMatrix;
	XMMATRIX tProj = XMLoadFloat4x4(&mProjectionMatrix);
	XMStoreFloat4x4(&rMatrix, XMMatrixTranspose( tProj));

	return rMatrix;
}