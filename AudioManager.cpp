#include "AudioSession.h"

AudioManager::AudioManager() : deviceEnumerator(nullptr), device(nullptr), audioEndpointVolume(nullptr), volumeCallback(nullptr) {
    volumeCallback = new AudioVolumeCallback(this);

    RegisterForNotifications(); // Register for device change notifications
}

AudioManager::~AudioManager() {
    UnregisterForNotifications(); // Clean up notifications
    if (audioEndpointVolume) audioEndpointVolume->Release();
    if (volumeCallback) volumeCallback->Release();
    if (device) device->Release();
    if (deviceEnumerator) deviceEnumerator->Release();
}

void AudioManager::RegisterForNotifications() {
    CoInitialize(NULL); // Initialize COM library
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);

    if (deviceEnumerator) {
        deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
        deviceEnumerator->RegisterEndpointNotificationCallback(this);
    }

    HRESULT hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&audioEndpointVolume);
    if (SUCCEEDED(hr) && volumeCallback) {
        audioEndpointVolume->RegisterControlChangeNotify(volumeCallback);
    }
}

void AudioManager::UnregisterForNotifications() {
    if (deviceEnumerator) {
        deviceEnumerator->UnregisterEndpointNotificationCallback(this);
    }

    if (audioEndpointVolume) {
        audioEndpointVolume->UnregisterControlChangeNotify(volumeCallback);
    }

    CoUninitialize(); // Clean up the COM library
}

// Implement the IMMNotificationClient methods
HRESULT AudioManager::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId) {
    // Handle default device change
    UnregisterForNotifications();

    // Re-register for notifications
    RegisterForNotifications();

    // Manually trigger volume change callback
    if (onVolumeChange) {
        onVolumeChange(GetVolume(), GetMuted());
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioManager::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) { return S_OK; }
HRESULT STDMETHODCALLTYPE AudioManager::OnDeviceAdded(LPCWSTR pwstrDeviceId) { return S_OK; }
HRESULT STDMETHODCALLTYPE AudioManager::OnDeviceRemoved(LPCWSTR pwstrDeviceId) { return S_OK; }
HRESULT STDMETHODCALLTYPE AudioManager::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) { return S_OK; }
HRESULT STDMETHODCALLTYPE AudioManager::QueryInterface(const IID& riid, void** ppvObject) { return S_OK; }
ULONG STDMETHODCALLTYPE AudioManager::AddRef() { return S_OK; }
ULONG STDMETHODCALLTYPE AudioManager::Release() { return S_OK; }


int AudioManager::GetVolume() {
    float volumeLevel = 0.0f;
    audioEndpointVolume->GetMasterVolumeLevelScalar(&volumeLevel); // Get volume level
    return static_cast<int>(round(volumeLevel * 100));
}

BOOL AudioManager::GetMuted() {
    BOOL isMuted;
    audioEndpointVolume->GetMute(&isMuted);
    return isMuted;
}

void AudioManager::SetVolumeChangeCallback(VolumeChangeCallback callback) {
    onVolumeChange = callback;
}
