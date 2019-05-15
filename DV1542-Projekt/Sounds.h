#pragma once

#include"Audio.h"
#include"Camera.h"
#include"Timer.h"
#include<SFML\Audio.hpp>

using namespace DirectX;
using namespace sf;
class Sounds
{
	//AudioEngine *audio_engine = new AudioEngine(AudioEngine_Debug);
	//
	//SoundEffect *jump;
	//
	//SoundEffect *bear;
	//SoundEffect *sphere;
	//SoundEffect *ambient;
	//std::unique_ptr<SoundEffectInstance> bear_instance;
	//std::unique_ptr<SoundEffectInstance> sphere_instance;
	//std::unique_ptr<SoundEffectInstance> ambient_instance;
	//
	//IXAudio2 *XAudio = NULL;
	//IXAudio2MasteringVoice *MasterVoice = NULL;

	SoundBuffer jump_buffer;
	SoundBuffer bear_buffer;
	SoundBuffer sphere_buffer;
	SoundBuffer ambient_buffer;

	Sound output;

	XMFLOAT3 bearPos;
	XMFLOAT3 spherePos;
	XMFLOAT3 camPos;

	bool jumping;

	Camera *cam;


public:
	Sounds(Camera *cam_ptr);
	void Update(bool isJumping);
	
	void UpdateBearSound();
	void UpdateSphereSound();

	~Sounds();

};

