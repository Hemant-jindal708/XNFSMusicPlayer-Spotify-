#pragma once
#ifndef SPOTIFY_CONTROLLER_H
#define SPOTIFY_CONTROLLER_H

#include <string>
#include <fstream>
#include <iostream>

// Forward declarations from Spotify code
extern std::string ReadToken(const std::string& tokenName);
extern bool SaveTokens(const std::string& accessToken, const std::string& refreshToken);
extern bool RefreshAccessToken(std::string& accessToken, const std::string& refreshToken);
extern void PerformInitialLogin();
extern void AutoLaunchSpotify();
extern std::string GetComputerDeviceID(std::string& accessToken, const std::string& refreshToken);
extern void ResumeSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);
extern void PauseSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);
extern void nextSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);
extern void previousSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);
extern void GetCurrentlyPlaying(std::string& accessToken, const std::string& refreshToken);
extern void DisplaySpotifyMetadata();

// Spotify Track Metadata Structure
struct SpotifyTrackInfo
{
    std::string trackName;
    std::string artistName;
    std::string albumName;
    std::string spotifyLogoText;  // For UI display
};

// Global Spotify state
class SpotifyController {
private:
    static SpotifyController* instance;
    std::string accessToken;
    std::string refreshToken;
    std::string deviceId;
    bool isInitialized;
    SpotifyTrackInfo currentTrack;

    SpotifyController();

public:
    static SpotifyController* GetInstance();
    
    // Initialize Spotify system
    bool Initialize();
    
    // Play/Pause/Skip commands
    void Play();
    void Pause();
    void NextTrack();
    void PreviousTrack();
    
    // UI and Metadata Functions
    void FetchCurrentTrackMetadata();
    SpotifyTrackInfo GetCurrentTrackInfo() const { return currentTrack; }
    void UpdateUIWithSpotifyMetadata();
    std::string FormatSpotifyDisplayText();
    
    // Token management
    void RefreshToken();
    bool IsInitialized() const { return isInitialized; }
    
    // Getters / setters
    std::string GetAccessToken() const { return accessToken; }
    std::string GetRefreshToken() const { return refreshToken; }
    std::string GetDeviceId() const { return deviceId; }
    void SetDeviceId(const std::string& newDeviceId) { deviceId = newDeviceId; }
};

// C-style wrapper functions for easier integration with C code
extern "C" {
    int SpotifyInit();
    void SpotifyPlay();
    void SpotifyPause();
    void SpotifyNext();
    void SpotifyPrevious();
    void SpotifyUpdateMetadata();
    const char* SpotifyGetTrackName();
    const char* SpotifyGetArtistName();
    const char* SpotifyGetAlbumName();
    int IsSpotifyInitialized();
    int SpotifyHasTrackChanged();
}

#endif // SPOTIFY_CONTROLLER_H
