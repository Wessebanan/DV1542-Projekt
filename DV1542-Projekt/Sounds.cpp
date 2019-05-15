#include "Sounds.h"

Sounds::Sounds(Camera* cam_ptr)
{

	this->cam = cam_ptr;
	bearPos = XMFLOAT3(160, -20, 180);
	spherePos = XMFLOAT3(512, 100, 512);

	/*jump = new SoundEffect(audio_engine, L"Jump.wav");

	bear = new SoundEffect(audio_engine, L"Bear.wav");
	sphere = new SoundEffect(audio_engine, L"Wind2.wav");
	ambient = new SoundEffect(audio_engine, L"Wind1.wav");
	
	bear_instance = bear->CreateInstance();
	sphere_instance = sphere->CreateInstance();
	ambient_instance = ambient->CreateInstance();	

	ambient_instance.get()->Play(true);
	ambient_instance.get()->SetVolume(0.5); */
}

void Sounds::Update(bool isJumping)
{
	if (isJumping && !jumping)
	{
		//jump->Play(0.5,0,0);
		jumping = true;
	}
	else if (!isJumping && jumping)
	{
		jumping = false;
	}

	UpdateBearSound();
	UpdateSphereSound();

}

void Sounds::UpdateBearSound()
{
	/*if (!bear_instance->IsLooped())
	{
		bear_instance->Play(true);
	}
*/
	XMFLOAT3 camToBear; 
	XMStoreFloat3(&camToBear, XMLoadFloat3(&bearPos) - cam->GetCamPosition());

	float bearDist = sqrt(pow(camToBear.x, 2) + pow(camToBear.y, 2) + pow(camToBear.z, 2));
	float bearVolume = 0.1f + 40.0f / bearDist;
	bearVolume = std::fmin(1.0f, bearVolume);

	// Getting the camera's forward vector in order
	// to determine the angle (x and y only as z would give misleading results).
	XMFLOAT3 camForward;
	XMStoreFloat3(&camForward, cam->GetForward());

	// Normalizing both vectors in x and y.
	float xy_bearDist = sqrt(pow(camToBear.x, 2) + pow(camToBear.z, 2));

	XMFLOAT3 n_camToBear = camToBear;
	n_camToBear.x /= xy_bearDist;
	n_camToBear.z /= xy_bearDist;

	float xy_forwardLength = sqrt(pow(camForward.x, 2) + pow(camForward.z, 2));

	XMFLOAT3 n_camForward = camForward;
	n_camForward.x /= xy_forwardLength;
	n_camForward.z /= xy_forwardLength;

	// Getting the cosine of the angle between the vectors in x and y, acting as the pan value.
	float cos_angle = n_camForward.x * n_camToBear.x + n_camForward.z * n_camToBear.z;

	/*bear_instance->SetPan(cos_angle);

	bear_instance->SetVolume(bearVolume);*/
}

void Sounds::UpdateSphereSound()
{
	/*if (!sphere_instance->IsLooped())
	{
		sphere_instance->Play(true);
	}*/

	// Getting a vector from the camera's position to the sphere 
	// in order to determine the distance.
	XMFLOAT3 camToSphere;
	XMStoreFloat3(&camToSphere, XMLoadFloat3(&spherePos) - cam->GetCamPosition());

	float sphereDist = sqrt(pow(camToSphere.x, 2) + pow(camToSphere.y, 2) + pow(camToSphere.z, 2));
	float sphereVolume = 0.1f + 50.0f / sphereDist;
	sphereVolume = std::fmin(1.0f, sphereVolume);

	// Getting the camera's forward vector in order
	// to determine the angle (x and y only as z would give misleading results).
	XMFLOAT3 camForward;
	XMStoreFloat3(&camForward, cam->GetForward());

	// Normalizing both vectors in x and y.
	float xy_sphereDist = sqrt(pow(camToSphere.x, 2) + pow(camToSphere.z, 2));

	XMFLOAT3 n_camToSphere = camToSphere;
	n_camToSphere.x /= xy_sphereDist;
	n_camToSphere.z /= xy_sphereDist;

	float xy_forwardLength = sqrt(pow(camForward.x, 2) + pow(camForward.z, 2));

	XMFLOAT3 n_camForward = camForward;
	n_camForward.x /= xy_forwardLength;
	n_camForward.z /= xy_forwardLength;
	
	// Getting the cosine of the angle between the vectors in x and y, acting as the pan value.
	float cos_angle = n_camForward.x * n_camToSphere.x + n_camForward.z * n_camToSphere.z;

	/*sphere_instance->SetPan(cos_angle);
	sphere_instance->SetVolume(sphereVolume);*/
}

Sounds::~Sounds()
{
	/*delete this->ambient;
	delete this->audio_engine;
	delete this->bear;
	delete this->sphere;
	delete this->jump;*/
}
