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

class AudioManager : public IMMNotificationClient {
public:
	using VolumeChangeCallback = std::function<void(int, BOOL)>;
	VolumeChangeCallback onVolumeChange;

	// IMMNotificationClient methods
	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId) override;
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) override;
	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId) override;
	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId) override;
	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override;
	HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;

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

	void RegisterForNotifications();
	void UnregisterForNotifications();
};

#endif // AUDIOSESSION_H
