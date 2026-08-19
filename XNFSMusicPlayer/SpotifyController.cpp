#include "SpotifyController.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <queue>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define PRINT_TYPE_INFO "[INFO]"
#define PRINT_TYPE_WARNING "[WARNING]"
#define PRINT_TYPE_ERROR "[ERROR]"
extern int XNFS_printf(unsigned int Verbosity, const char* format, ...);

// Global storage for C functions
static char g_trackName[256] = "Spotify";
static char g_artistName[256] = "No Track";
static char g_albumName[256] = "";
static char g_lastTrackName[256] = "";  // For detecting track changes
static volatile bool g_trackChanged = false;

namespace
{
    enum class SpotifyAction
    {
        Play,
        Pause,
        Next,
        Previous,
        RefreshMetadata,
        RefreshMetadataAndUi
    };

    static std::queue<SpotifyAction> g_spotifyQueue;
    static CRITICAL_SECTION g_spotifyQueueLock;
    static HANDLE g_spotifyRequestThread = nullptr;
    static bool g_spotifyQueueInitialized = false;

    void QueueSpotifyAction(SpotifyAction action)
    {
        if (!g_spotifyQueueInitialized)
        {
            InitializeCriticalSection(&g_spotifyQueueLock);
            g_spotifyQueueInitialized = true;
        }

        EnterCriticalSection(&g_spotifyQueueLock);
        g_spotifyQueue.push(action);
        LeaveCriticalSection(&g_spotifyQueueLock);

        if (!g_spotifyRequestThread)
        {
            g_spotifyRequestThread = CreateThread(nullptr, 0, [](LPVOID)->DWORD {
                while (true)
                {
                    SpotifyAction pendingAction = SpotifyAction::RefreshMetadata;

                    EnterCriticalSection(&g_spotifyQueueLock);
                    if (g_spotifyQueue.empty())
                    {
                        LeaveCriticalSection(&g_spotifyQueueLock);
                        Sleep(25);
                        continue;
                    }

                    pendingAction = g_spotifyQueue.front();
                    g_spotifyQueue.pop();
                    LeaveCriticalSection(&g_spotifyQueueLock);

                    SpotifyController* controller = SpotifyController::GetInstance();
                    if (!controller || !controller->IsInitialized())
                        continue;

                    std::string accessToken = controller->GetAccessToken();
                    std::string refreshToken = controller->GetRefreshToken();
                    std::string deviceId = controller->GetDeviceId();

                    switch (pendingAction)
                    {
                    case SpotifyAction::Play:
                        if (accessToken.empty())
                        {
                            std::cout << "[Spotify] Play: Access token empty!" << std::endl;
                            break;
                        }
                        if (deviceId.empty())
                        {
                            std::cout << "[Spotify] Play: Device ID empty, launching Spotify..." << std::endl;
                            AutoLaunchSpotify();
                            Sleep(5000);
                            deviceId = GetComputerDeviceID(accessToken, refreshToken);
                            if (!deviceId.empty())
                            {
                                controller->SetDeviceId(deviceId);
                                std::cout << "[Spotify] Play: Device ID acquired: " << deviceId << std::endl;
                            }
                            else
                            {
                                std::cout << "[Spotify] Play: Failed to get device ID" << std::endl;
                                break;
                            }
                        }
                        std::cout << "[Spotify] Sending play command..." << std::endl;
                        ResumeSong(accessToken, refreshToken, deviceId);
                        Sleep(100);
                        controller->FetchCurrentTrackMetadata();
                        
                        // Force UI to show by marking the track as changed
                        g_trackChanged = true;
                        
                        std::cout << "[Spotify] Play command completed." << std::endl;
                        break;
                    case SpotifyAction::Pause:
                        if (accessToken.empty())
                        {
                            std::cout << "[Spotify] Pause: Access token empty!" << std::endl;
                            break;
                        }
                        if (deviceId.empty())
                        {
                            std::cout << "[Spotify] Pause: Device ID empty, launching Spotify..." << std::endl;
                            AutoLaunchSpotify();
                            Sleep(5000);
                            deviceId = GetComputerDeviceID(accessToken, refreshToken);
                            if (!deviceId.empty())
                            {
                                controller->SetDeviceId(deviceId);
                                std::cout << "[Spotify] Pause: Device ID acquired: " << deviceId << std::endl;
                            }
                            else
                            {
                                std::cout << "[Spotify] Pause: Failed to get device ID" << std::endl;
                                break;
                            }
                        }
                        std::cout << "[Spotify] Sending pause command..." << std::endl;
                        PauseSong(accessToken, refreshToken, deviceId);
                        Sleep(100);
                        controller->FetchCurrentTrackMetadata();
                        std::cout << "[Spotify] Pause command completed." << std::endl;
                        break;
                    case SpotifyAction::Next:
                        if (accessToken.empty())
                            break;
                        if (deviceId.empty())
                        {
                            std::cout << "[Spotify] Skipping: Device ID empty, launching Spotify..." << std::endl;
                            AutoLaunchSpotify();
                            Sleep(5000);
                            deviceId = GetComputerDeviceID(accessToken, refreshToken);
                            if (!deviceId.empty())
                            {
                                controller->SetDeviceId(deviceId);
                                std::cout << "[Spotify] Skipping: Device ID acquired: " << deviceId << std::endl;
                            }
                            else
                            {
                                std::cout << "[Spotify] Skipping: Failed to get device ID" << std::endl;
                                break;
                            }
                        }
                        std::cout << "[Spotify] Sending skip next command..." << std::endl;
                        nextSong(accessToken, refreshToken, deviceId);
                        Sleep(250);  // Reduced from 1000ms
                        controller->FetchCurrentTrackMetadata();
                        std::cout << "[Spotify] Skip next command completed." << std::endl;
                        break;
                    case SpotifyAction::Previous:
                        if (accessToken.empty())
                            break;
                        if (deviceId.empty())
                        {
                            std::cout << "[Spotify] Going to previous: Device ID empty, launching Spotify..." << std::endl;
                            AutoLaunchSpotify();
                            Sleep(5000);
                            deviceId = GetComputerDeviceID(accessToken, refreshToken);
                            if (!deviceId.empty())
                            {
                                controller->SetDeviceId(deviceId);
                                std::cout << "[Spotify] Going to previous: Device ID acquired: " << deviceId << std::endl;
                            }
                            else
                            {
                                std::cout << "[Spotify] Going to previous: Failed to get device ID" << std::endl;
                                break;
                            }
                        }
                        std::cout << "[Spotify] Sending previous command..." << std::endl;
                        previousSong(accessToken, refreshToken, deviceId);
                        Sleep(250);  // Reduced from 1000ms
                        controller->FetchCurrentTrackMetadata();
                        std::cout << "[Spotify] Previous command completed." << std::endl;
                        break;
                    case SpotifyAction::RefreshMetadata:
                        controller->FetchCurrentTrackMetadata();
                        break;
                    case SpotifyAction::RefreshMetadataAndUi:
                        controller->FetchCurrentTrackMetadata();
                        break;
                    }
                }
            }, nullptr, 0, nullptr);
        }
    }
}

SpotifyController* SpotifyController::instance = nullptr;

SpotifyController::SpotifyController()
    : isInitialized(false)
{
    currentTrack.spotifyLogoText = "[Spotify]";
}

SpotifyController* SpotifyController::GetInstance()
{
    if (instance == nullptr) {
        instance = new SpotifyController();
    }
    return instance;
}

bool SpotifyController::Initialize()
{
    if (isInitialized)
        return true;

    try
    {
        // Read tokens from ini file
        accessToken = ReadToken("ACCESS");
        refreshToken = ReadToken("REFRESH");

        // If no tokens exist, perform browser login
        if (refreshToken.empty()) {
            XNFS_printf(1, "%s: Spotify No tokens found. Starting browser login...\n", PRINT_TYPE_INFO);
            PerformInitialLogin();
            accessToken = ReadToken("ACCESS");
            refreshToken = ReadToken("REFRESH");

            if (refreshToken.empty()) {
                XNFS_printf(1, "%s: Spotify Login cancelled or failed. Spotify integration disabled.\n", PRINT_TYPE_WARNING);
                return false;
            }
        }

        // Refresh token to ensure it's valid
        if (!RefreshAccessToken(accessToken, refreshToken)) {
            XNFS_printf(1, "%s: Spotify Failed to refresh access token. Token may be expired. Starting browser login...\n", PRINT_TYPE_WARNING);
            PerformInitialLogin();
            accessToken = ReadToken("ACCESS");
            refreshToken = ReadToken("REFRESH");
            
            if (refreshToken.empty() || !RefreshAccessToken(accessToken, refreshToken)) {
                XNFS_printf(1, "%s: Spotify Re-login cancelled or failed. Spotify integration disabled.\n", PRINT_TYPE_ERROR);
                return false;
            }
        }

        // Do not launch Spotify or resume playback during game startup.
        // Spotify should only become active when the player actually requests playback or a transport action.
        XNFS_printf(1, "%s: Spotify fetching device ID...\n", PRINT_TYPE_INFO);
        deviceId = GetComputerDeviceID(accessToken, refreshToken);
        if (!deviceId.empty())
        {
            std::ofstream file("spotify_tokens.ini", std::ios::app);
            if (file.is_open())
            {
                file << "DEVICE_ID=" << deviceId << "\n";
                file.close();
            }
            XNFS_printf(1, "%s: Spotify Device ID: %s\n", PRINT_TYPE_INFO, deviceId.c_str());
        }
        else
        {
            XNFS_printf(1, "%s: Spotify Device ID not ready yet. Spotify will activate on the next playback request.\n", PRINT_TYPE_INFO);
        }

        isInitialized = true;
        XNFS_printf(1, "%s: Spotify Initialization complete. Fetching metadata...\n", PRINT_TYPE_INFO);

        FetchCurrentTrackMetadata();
        
        XNFS_printf(1, "%s: Spotify Initialization complete. No auto-play on boot.\n", PRINT_TYPE_INFO);
        
        return true;
    }
    catch (const std::exception& e)
    {
        XNFS_printf(1, "%s: EXCEPTION IN SpotifyController::Initialize: %s\n", PRINT_TYPE_ERROR, e.what());
        return false;
    }
    catch (...)
    {
        XNFS_printf(1, "%s: UNKNOWN EXCEPTION IN SpotifyController::Initialize\n", PRINT_TYPE_ERROR);
        return false;
    }
}

void SpotifyController::Play()
{
    if (!isInitialized) {
        std::cout << "[Spotify] Not initialized. Call Initialize() first." << std::endl;
        return;
    }

    QueueSpotifyAction(SpotifyAction::Play);
}

void SpotifyController::Pause()
{
    if (!isInitialized) {
        std::cout << "[Spotify] Not initialized. Call Initialize() first." << std::endl;
        return;
    }

    QueueSpotifyAction(SpotifyAction::Pause);
}

void SpotifyController::NextTrack()
{
    if (!isInitialized) {
        std::cout << "[Spotify] Not initialized. Call Initialize() first." << std::endl;
        return;
    }

    QueueSpotifyAction(SpotifyAction::Next);
}

void SpotifyController::PreviousTrack()
{
    if (!isInitialized) {
        std::cout << "[Spotify] Not initialized. Call Initialize() first." << std::endl;
        return;
    }

    QueueSpotifyAction(SpotifyAction::Previous);
}

void SpotifyController::FetchCurrentTrackMetadata()
{
    if (!isInitialized) return;

    std::cout << "[Spotify] Fetching current track metadata..." << std::endl;
    
    // Make HTTP request to get currently playing track
    cpr::Response r = cpr::Get(
        cpr::Url{"https://api.spotify.com/v1/me/player/currently-playing"},
        cpr::Header{{"Authorization", "Bearer " + accessToken}}
    );

    if (r.status_code == 200) {
        try {
            json responseJson = json::parse(r.text);
            if (!responseJson["item"].is_null()) {
                currentTrack.trackName = responseJson["item"]["name"].get<std::string>();
                
                // Get artist name
                if (responseJson["item"]["artists"].size() > 0) {
                    currentTrack.artistName = responseJson["item"]["artists"][0]["name"].get<std::string>();
                }
                
                // Get album name
                if (!responseJson["item"]["album"]["name"].is_null()) {
                    currentTrack.albumName = responseJson["item"]["album"]["name"].get<std::string>();
                }
                
                // Copy to global buffers for C functions
                strncpy_s(g_trackName, sizeof(g_trackName), currentTrack.trackName.c_str(), _TRUNCATE);
                strncpy_s(g_artistName, sizeof(g_artistName), currentTrack.artistName.c_str(), _TRUNCATE);
                strncpy_s(g_albumName, sizeof(g_albumName), currentTrack.albumName.c_str(), _TRUNCATE);

                // Check if track has changed and log it
                if (strcmp(g_lastTrackName, g_trackName) != 0) {
                    XNFS_printf(2, "%s: Spotify TRACK CHANGED: %s - %s\n", PRINT_TYPE_INFO, g_artistName, g_trackName);
                    strncpy_s(g_lastTrackName, sizeof(g_lastTrackName), g_trackName, _TRUNCATE);
                    // The main game thread (SoundUpdater) will pick this up and call DisplaySpotifyMetadata safely
                    g_trackChanged = true;
                } else {
                    XNFS_printf(3, "%s: Spotify Track Updated: %s - %s\n", PRINT_TYPE_INFO, currentTrack.artistName.c_str(), currentTrack.trackName.c_str());
                }
            }
        }
        catch (const std::exception& e) {
            XNFS_printf(1, "%s: Spotify Error parsing track metadata: %s\n", PRINT_TYPE_ERROR, e.what());
        }
    } else if (r.status_code == 401) {
        // Token expired, refresh it
        RefreshAccessToken(accessToken, refreshToken);
        FetchCurrentTrackMetadata();  // Retry
    } else if (r.status_code == 204) {
        XNFS_printf(2, "%s: Spotify No track currently playing.\n", PRINT_TYPE_INFO);
        currentTrack.trackName = "No Track Playing";
        currentTrack.artistName = "Spotify";
        currentTrack.albumName = "";
    } else {
        XNFS_printf(1, "%s: Spotify Error fetching track metadata. Code: %d\n", PRINT_TYPE_ERROR, r.status_code);
    }
}

void SpotifyController::UpdateUIWithSpotifyMetadata()
{
    // This function will be called from the UI update functions
    // The metadata is already in currentTrack and g_* buffers
    // The UI will read from those buffers
}

std::string SpotifyController::FormatSpotifyDisplayText()
{
    std::string result = "[Spotify] ";
    result += currentTrack.artistName;
    result += " - ";
    result += currentTrack.trackName;
    return result;
}

void SpotifyController::RefreshToken()
{
    if (!isInitialized) return;
    RefreshAccessToken(accessToken, refreshToken);
}

// C-style wrapper functions
int SpotifyInit()
{
    SpotifyController* controller = SpotifyController::GetInstance();
    return controller->Initialize() ? 1 : 0;
}

void SpotifyPlay()
{
    SpotifyController* controller = SpotifyController::GetInstance();
    if (controller->IsInitialized())
        controller->Play();
}

void SpotifyPause()
{
    SpotifyController* controller = SpotifyController::GetInstance();
    if (controller->IsInitialized())
        controller->Pause();
}

void SpotifyNext()
{
    SpotifyController* controller = SpotifyController::GetInstance();
    if (controller->IsInitialized())
        controller->NextTrack();
}

void SpotifyPrevious()
{
    SpotifyController* controller = SpotifyController::GetInstance();
    if (controller->IsInitialized())
        controller->PreviousTrack();
}

void SpotifyUpdateMetadata()
{
    SpotifyController* controller = SpotifyController::GetInstance();
    if (controller->IsInitialized())
        QueueSpotifyAction(SpotifyAction::RefreshMetadata);
}

const char* SpotifyGetTrackName()
{
    return g_trackName;
}

const char* SpotifyGetArtistName()
{
    return g_artistName;
}

const char* SpotifyGetAlbumName()
{
    return g_albumName;
}

int IsSpotifyInitialized()
{
    SpotifyController* controller = SpotifyController::GetInstance();
    return controller->IsInitialized() ? 1 : 0;
}

int SpotifyHasTrackChanged()
{
    if (g_trackChanged)
    {
        g_trackChanged = false;
        return 1;
    }
    return 0;
}
