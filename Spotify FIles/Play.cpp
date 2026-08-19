#include "play.h"

// --- Get Windows Local Computer Name ---
static std::string GetLocalComputerName() {
    char buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(buffer);
    
    if (GetComputerNameA(buffer, &size)) {
        return std::string(buffer);
    }
    return "UNKNOWN";
}

// --- Auto-Launch Spotify Silently ---
void AutoLaunchSpotify() {
    std::cout << "Launching Spotify in the background..." << std::endl;

    const char* fallbackPaths[] = {
        "spotify://",
        "C:\\Users\\%USERNAME%\\AppData\\Roaming\\Spotify\\Spotify.exe",
        "C:\\Program Files\\Spotify\\Spotify.exe",
        "C:\\Program Files (x86)\\Spotify\\Spotify.exe"
    };

    for (const auto& path : fallbackPaths)
    {
        if (ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOWMINNOACTIVE) > (HINSTANCE)32)
        {
            std::cout << "[Spotify] App launch command accepted for: " << path << std::endl;
            return;
        }
    }

    std::cout << "[Spotify] Could not launch Spotify via shell. The desktop app may not be installed." << std::endl;
}

// --- Unified HTTP Request Handler with 401 Auto-Refresh & Retry ---
cpr::Response SendSpotifyRequest(
    const std::string& method,
    const std::string& url,
    std::string& accessToken,
    const std::string& refreshToken)
{
    auto makeRequest = [&](const std::string& token) {
        cpr::Header headers = {
            {"Authorization", "Bearer " + token}
        };

        if (method == "PUT" || method == "POST") {
            headers.insert({"Content-Length", "0"});
        }

        if (method == "GET") {
            return cpr::Get(cpr::Url{url}, headers);
        } else if (method == "PUT") {
            return cpr::Put(cpr::Url{url}, headers);
        } else if (method == "POST") {
            return cpr::Post(cpr::Url{url}, headers);
        }
        return cpr::Response{};
    };

    // Attempt 1: Try request with current Access Token
    cpr::Response response = makeRequest(accessToken);

    // If 401 Unauthorized occurs, refresh token and retry request automatically
    if (response.status_code == 401 && !refreshToken.empty()) {
        std::cout << "\n[HTTP 401] Access token expired! Auto-refreshing..." << std::endl;
        if (RefreshAccessToken(accessToken, refreshToken)) {
            std::cout << "[HTTP Retry] Retrying request with new Access Token..." << std::endl;
            response = makeRequest(accessToken);
        }
    }

    return response;
}

// Helper to safely attach Device ID to Endpoint URLs
static std::string TargetDeviceUrl(const std::string& baseEndpoint, const std::string& deviceId) {
    if (!deviceId.empty()) {
        return baseEndpoint + "?device_id=" + deviceId;
    }
    return baseEndpoint;
}

// --- Fetch Local PC Device ID ---
std::string GetComputerDeviceID(std::string& accessToken, const std::string& refreshToken) {
    std::string myHostName = GetLocalComputerName();
    std::cout << "Searching for Spotify device matching local PC: " << myHostName << "..." << std::endl;

    cpr::Response r = SendSpotifyRequest(
        "GET",
        "https://api.spotify.com/v1/me/player/devices",
        accessToken,
        refreshToken
    );

    if (r.status_code == 200) {
        json responseJson = json::parse(r.text);
        std::string fallbackComputerId = "";
        
        if (responseJson.contains("devices")) {
            for (const auto& device : responseJson["devices"]) {
                std::string deviceType = device.value("type", "");
                std::string deviceName = device.value("name", "");
                std::string deviceId = device.value("id", "");

                if (deviceType == "Computer") {
                    if (fallbackComputerId.empty()) fallbackComputerId = deviceId;

                    if (deviceName.find(myHostName) != std::string::npos) {
                        std::cout << "Exact match found! Targeting: " << deviceName << std::endl;
                        return deviceId;
                    }
                }
            }
        }
        
        if (!fallbackComputerId.empty()) {
            std::cout << "No exact name match, defaulting to first available computer." << std::endl;
            return fallbackComputerId;
        }

        std::cout << "Warning: No computer found. Is Spotify open?" << std::endl;
    } else {
        std::cout << "Error fetching devices: " << r.status_code << "\n" << r.text << std::endl;
    }

    return ""; 
}

// --- Playback Commands ---

void GetCurrentlyPlaying(std::string& accessToken, const std::string& refreshToken) {
    std::cout << "\nFetching currently playing song..." << std::endl;
    
    cpr::Response r = SendSpotifyRequest(
        "GET",
        "https://api.spotify.com/v1/me/player/currently-playing",
        accessToken,
        refreshToken
    );

    if (r.status_code == 200) {
        json responseJson = json::parse(r.text);
        if (!responseJson["item"].is_null()) {
            std::string songTitle = responseJson["item"]["name"];
            std::string artist = responseJson["item"]["artists"][0]["name"];
            
            std::cout << "--------------------------------------" << std::endl;
            std::cout << "NOW PLAYING: " << artist << " - " << songTitle << std::endl;
            std::cout << "--------------------------------------\n" << std::endl;
        }
    } else if (r.status_code == 204) {
        std::cout << "Nothing is currently playing (Code 204)." << std::endl;
    } else {
        std::cout << "Error fetching song. Code: " << r.status_code << "\n" << r.text << std::endl;
    }
}

void ResumeSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId) {
    std::cout << "Sending 'Play' command..." << std::endl;
    
    cpr::Response r = SendSpotifyRequest(
        "PUT",
        TargetDeviceUrl("https://api.spotify.com/v1/me/player/play", deviceId),
        accessToken,
        refreshToken
    );

    if (r.status_code == 204 || r.status_code == 200) {
        std::cout << "Successfully resumed playback!" << std::endl;
    } else {
        std::cout << "Error playing song. Code: " << r.status_code << "\n" << r.text << std::endl;
    }
}

void PauseSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId) {
    std::cout << "Sending 'Pause' command..." << std::endl;
    
    cpr::Response r = SendSpotifyRequest(
        "PUT",
        TargetDeviceUrl("https://api.spotify.com/v1/me/player/pause", deviceId),
        accessToken,
        refreshToken
    );

    if (r.status_code == 204 || r.status_code == 200) {
        std::cout << "Successfully paused playback!" << std::endl;
    } else {
        std::cout << "Error pausing song. Code: " << r.status_code << "\n" << r.text << std::endl;
    }
}

void nextSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId) {
    std::cout << "Sending 'Next' command..." << std::endl;
    
    cpr::Response r = SendSpotifyRequest(
        "POST",
        TargetDeviceUrl("https://api.spotify.com/v1/me/player/next", deviceId),
        accessToken,
        refreshToken
    );

    if (r.status_code == 204 || r.status_code == 200) {
        std::cout << "Successfully skipped to the next song!" << std::endl;
    } else {
        std::cout << "Error skipping song. Code: " << r.status_code << "\n" << r.text << std::endl;
    }
}

void previousSong(std::string& accessToken, const std::string& refreshToken, const std::string& deviceId) {
    std::cout << "Sending 'Previous' command..." << std::endl;
    
    cpr::Response r = SendSpotifyRequest(
        "POST",
        TargetDeviceUrl("https://api.spotify.com/v1/me/player/previous", deviceId),
        accessToken,
        refreshToken
    );

    if (r.status_code == 204 || r.status_code == 200) {
        std::cout << "Successfully skipped to the previous song!" << std::endl;
    } else {
        std::cout << "Error skipping backwards. Code: " << r.status_code << "\n" << r.text << std::endl;
    }
}