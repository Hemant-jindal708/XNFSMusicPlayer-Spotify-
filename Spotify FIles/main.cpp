#include "play.h"
#include <httplib.h>

#pragma comment(lib, "Shell32.lib")

// --- 1. Read token from spotify_tokens.ini ---
std::string ReadToken(const std::string &tokenName) {
  std::ifstream file("spotify_tokens.ini");
  std::string line;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::string prefix = tokenName + "=";
      if (line.find(prefix) == 0) {
        return line.substr(prefix.length());
      }
    }
    file.close();
  }
  return "";
}

// --- 2. Save tokens to spotify_tokens.ini ---
bool SaveTokens(const std::string &accessToken,
                const std::string &refreshToken) {
  std::ofstream file("spotify_tokens.ini");
  if (!file.is_open()) {
    return false;
  }

  file << "ACCESS=" << accessToken << "\n";
  file << "REFRESH=" << refreshToken << "\n";
  file.close();
  return true;
}

// --- 3. Refresh Access Token using Refresh Token ---
bool RefreshAccessToken(std::string &accessToken,
                        const std::string &refreshToken) {
  if (refreshToken.empty()) {
    std::cout << "[Auth] No refresh token available!" << std::endl;
    return false;
  }

  std::cout << "[Auth] Refreshing Access Token from Spotify..." << std::endl;

  cpr::Response r = cpr::Post(
      cpr::Url{"https://accounts.spotify.com/api/token"},
      cpr::Payload{{"grant_type", "refresh_token"},
                   {"refresh_token", refreshToken},
                   {"client_id", CLIENT_ID},
                   {"client_secret", CLIENT_SECRET}},
      cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"}});

  if (r.status_code == 200) {
    json responseJson = json::parse(r.text);
    if (responseJson.contains("access_token")) {
      accessToken = responseJson["access_token"].get<std::string>();
      std::cout << "[Auth] Access Token refreshed and saved successfully!"
                << std::endl;
      SaveTokens(accessToken, refreshToken);
      return true;
    }
  }

  std::cout << "[Auth] Failed to refresh token. Code: " << r.status_code << "\n"
            << r.text << std::endl;
  return false;
}

// --- 4. OAuth Browser Login Flow (If no INI file exists) ---
void PerformInitialLogin() {
  std::string scopes = "user-read-currently-playing%20user-modify-playback-"
                       "state%20user-read-playback-state";
  std::string authUrl =
      "https://accounts.spotify.com/authorize?client_id=" + CLIENT_ID +
      "&response_type=code&redirect_uri=" + REDIRECT_URI + "&scope=" + scopes;

  std::cout << "[Auth] Opening web browser for initial login..." << std::endl;
  ShellExecuteA(NULL, "open", authUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);

  httplib::Server svr;
  std::string authCode = "";

  svr.Get("/callback", [&](const httplib::Request &req,
                           httplib::Response &res) {
    if (req.has_param("code")) {
      authCode = req.get_param_value("code");
      res.set_content(
          "Login Successful! You can close this tab and return to the game.",
          "text/plain");
      svr.stop();
    } else {
      res.set_content("Login Failed.", "text/plain");
      svr.stop();
    }
  });

  std::cout << "[Auth] Waiting for authentication on port 8080..." << std::endl;
  svr.listen("127.0.0.1", 8080);

  if (!authCode.empty()) {
    std::cout << "[Auth] Exchanging auth code for tokens..." << std::endl;
    cpr::Response r = cpr::Post(
        cpr::Url{"https://accounts.spotify.com/api/token"},
        cpr::Payload{{"grant_type", "authorization_code"},
                     {"code", authCode},
                     {"redirect_uri", REDIRECT_URI},
                     {"client_id", CLIENT_ID},
                     {"client_secret", CLIENT_SECRET}},
        cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"}});

    if (r.status_code == 200) {
      json responseJson = json::parse(r.text);
      std::string accessToken = responseJson["access_token"];
      std::string refreshToken = responseJson["refresh_token"];
      SaveTokens(accessToken, refreshToken);
      std::cout << "[Auth] Initial tokens saved to spotify_tokens.ini!"
                << std::endl;
    } else {
      std::cout << "[Auth] Failed to obtain initial tokens. Code: "
                << r.status_code << "\n"
                << r.text << std::endl;
    }
  }
}

// int main() {
//     std::cout << "Loading Spotify Mod Controller...\n" << std::endl;

//     std::string accessToken = ReadToken("ACCESS");
//     std::string refreshToken = ReadToken("REFRESH");

//     // If no tokens exist, perform browser login
//     if (refreshToken.empty()) {
//         std::cout << "[Auth] No tokens found. Starting browser login..." <<
//         std::endl; PerformInitialLogin(); accessToken = ReadToken("ACCESS");
//         refreshToken = ReadToken("REFRESH");

//         if (refreshToken.empty()) {
//             std::cout << "[Error] Login cancelled or failed. Exiting." <<
//             std::endl; return 1;
//         }
//     }

//     // Always ensure token is fresh on launch
//     if (accessToken.empty() || !refreshToken.empty()) {
//         RefreshAccessToken(accessToken, refreshToken);
//     }

//     std::cout << "Access Token active and ready!\n" << std::endl;

//     // --- EXECUTE PLAYBACK TEST ---
//     AutoLaunchSpotify();

//     std::cout << "Waiting for Spotify to boot and connect..." << std::endl;
//     Sleep(5000);

//     std::string myDeviceId = GetComputerDeviceID(accessToken, refreshToken);

//     // ResumeSong(accessToken, refreshToken, myDeviceId);
//     // Sleep(500);
//     // GetCurrentlyPlaying(accessToken, refreshToken);

//     // Sleep(3000);
//     // PauseSong(accessToken, refreshToken, myDeviceId);

//     // Sleep(2000);
//     // nextSong(accessToken, refreshToken, myDeviceId);
//     // Sleep(500);
//     // GetCurrentlyPlaying(accessToken, refreshToken);

//     // previousSong(accessToken, refreshToken, myDeviceId);
//     // Sleep(500);
//     // GetCurrentlyPlaying(accessToken, refreshToken);

//     // PauseSong(accessToken, refreshToken, myDeviceId);

//     return 0;
// }