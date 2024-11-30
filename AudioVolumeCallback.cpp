#include "AudioSession.h"

AudioVolumeCallback::AudioVolumeCallback(AudioManager* manager) : _cRef(1), audioManager(manager) {}

AudioVolumeCallback::~AudioVolumeCallback() {}

ULONG STDMETHODCALLTYPE AudioVolumeCallback::AddRef() {
    return InterlockedIncrement(&_cRef);
}

ULONG STDMETHODCALLTYPE AudioVolumeCallback::Release() {
    ULONG ulRef = InterlockedDecrement(&_cRef);
    if (0 == ulRef) delete this;
    return ulRef;
}

HRESULT STDMETHODCALLTYPE AudioVolumeCallback::QueryInterface(REFIID riid, VOID** ppvInterface) {
    if (IID_IUnknown == riid || __uuidof(IAudioEndpointVolumeCallback) == riid) {
        AddRef();
        *ppvInterface = static_cast<IAudioEndpointVolumeCallback*>(this);
        return S_OK;
    }
    *ppvInterface = NULL;
    return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE AudioVolumeCallback::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
    if (pNotify && audioManager->onVolumeChange) {
        int volume = static_cast<int>(round(pNotify->fMasterVolume * 100));
        BOOL isMuted = pNotify->bMuted;
        audioManager->onVolumeChange(volume, isMuted);
    }
    return S_OK;
}
