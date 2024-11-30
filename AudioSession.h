#ifndef AUDIOSESSION_H
#define AUDIOSESSION_H

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functional>

class AudioManager; // Forward-Looking Statement

class AudioVolumeCallback : public IAudioEndpointVolumeCallback {
public:
	AudioVolumeCallback(AudioManager* manager);
	~AudioVolumeCallback();

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);
	HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);

private:
	LONG _cRef;
	AudioManager* audioManager;
};

class AudioManager {
public:
	using VolumeChangeCallback = std::function<void(int, BOOL)>;
	VolumeChangeCallback onVolumeChange;

	AudioManager();
	~AudioManager();
	int GetVolume();
	BOOL GetMuted();
	void SetVolumeChangeCallback(VolumeChangeCallback callback);

private:
	IMMDeviceEnumerator* deviceEnumerator;
	IMMDevice* device;
	IAudioEndpointVolume* audioEndpointVolume;
	AudioVolumeCallback* volumeCallback;
};

#endif // AUDIOSESSION_H
