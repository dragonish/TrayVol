#include "AudioSession.h"

AudioManager::AudioManager() : deviceEnumerator(nullptr), device(nullptr), audioEndpointVolume(nullptr), volumeCallback(nullptr) {
    CoInitialize(NULL); // Initialize COM library
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
    device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
        NULL, (void**)&audioEndpointVolume);

    volumeCallback = new AudioVolumeCallback(this);
    audioEndpointVolume->RegisterControlChangeNotify(volumeCallback);
}

AudioManager::~AudioManager() {
    if (audioEndpointVolume) audioEndpointVolume->Release();
    if (volumeCallback) volumeCallback->Release();
    if (device) device->Release();
    if (deviceEnumerator) deviceEnumerator->Release();
    CoUninitialize(); // Clean up the COM library
}

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
