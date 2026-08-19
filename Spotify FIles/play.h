#ifndef PLAY_H
#define PLAY_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// --- CREDENTIALS ---
const std::string CLIENT_ID = "8959f9ba21e04e30a067efc2e54d6fc0";
const std::string CLIENT_SECRET = "be7aa9868afc40aeb540befb27f73dc4";
const std::string REDIRECT_URI = "http://127.0.0.1:8080/callback";

// Token Management Functions (Defined in main.cpp)
std::string ReadToken(const std::string& tokenName);
bool SaveTokens(const std::string& accessToken, const std::string& refreshToken);
bool RefreshAccessToken(std::string& accessToken, const std::string& refreshToken);
void PerformInitialLogin();

// Playback Operations (Defined in play.cpp)
void AutoLaunchSpotify();
cpr::Response SendSpotifyRequest(
    const std::string& method,
    const std::string& url,
    std::string& accessToken,
    const std::string& refreshToken
);
std::string GetComputerDeviceID(std::string& accessToken, const std::string& refreshToken);
void GetCurrentlyPlaying(std::string& accessToken, const std::string& refreshToken);
void ResumeSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);
void PauseSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);
void nextSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);
void previousSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId);

#endif