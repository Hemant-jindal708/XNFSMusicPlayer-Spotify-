# XNFSMusicPlayer Spotify Integration Guide

## Overview
This guide explains how to enable and configure Spotify playback in the XNFSMusicPlayer mod for NFS MW 2005.

## Features
- **Spotify Playback**: Play music from Spotify instead of local MP3 files
- **Token Management**: Automatic OAuth2 authentication with Spotify
- **Device Detection**: Automatically detects and uses your computer as the Spotify device
- **Play/Pause Control**: Game controls are forwarded to Spotify API
- **Fallback Support**: Automatically falls back to local MP3s if Spotify is unavailable

## Setup Instructions

### Step 1: Enable Spotify in Configuration
Edit the `XNFSMusicPlayer.ini` file in your game's scripts directory and add the following section:

```ini
[Spotify]
EnableSpotify=1
```

### Step 2: Initial Authentication
1. Run the game with Spotify integration enabled
2. A web browser will automatically open for Spotify OAuth login
3. Log in with your Spotify account
4. Grant the required permissions when prompted
5. You will be redirected back to confirm successful login

### Step 3: Token Storage
The mod automatically saves your authentication tokens in `spotify_tokens.ini`:
```ini
ACCESS=<your_access_token>
REFRESH=<your_refresh_token>
DEVICE_ID=<your_computer_device_id>
```

**Important**: Keep `spotify_tokens.ini` secure - it contains tokens for your Spotify account!

### Step 4: Launch Spotify
Make sure Spotify is running before playing music through the mod. The mod will automatically detect your computer as a playback device.

## How It Works

### Architecture
- **SpotifyController.cpp/h**: C++ wrapper that manages Spotify authentication and API calls
- **play.h / play.cpp**: Original Spotify library code for API communication
- **main.cpp**: Initial authentication flow (OAuth browser login)

### Play/Pause Flow
1. Game calls pause/resume functions in the mod
2. The mod checks if Spotify is enabled and initialized
3. If yes: Forwards the command to Spotify API via `SpotifyController`
4. If no: Falls back to traditional BASS audio library playback

### Functions Integrated
- **SpotifyPlay()** - Resumes playback on Spotify device
- **SpotifyPause()** - Pauses playback on Spotify device
- **SpotifyNext()** - Skips to next track (available for custom hotkeys)
- **SpotifyPrevious()** - Skips to previous track (available for custom hotkeys)

## Troubleshooting

### "Spotify initialization failed" message
- Ensure Spotify is running and logged in
- Check that your tokens in `spotify_tokens.ini` are valid
- Delete `spotify_tokens.ini` to trigger re-authentication

### Playback not working
- Verify Spotify is playing on your computer (the mod controls this device)
- Check the console output for error messages
- Ensure your device is properly selected in the `DEVICE_ID` in `spotify_tokens.ini`

### Token expiration
- The mod automatically refreshes expired tokens using the refresh token
- If authentication fails completely, delete `spotify_tokens.ini` and restart

## Configuration Options

### XNFSMusicPlayer.ini
```ini
[Spotify]
EnableSpotify=1    ; Set to 1 to enable Spotify, 0 to disable
```

### Advanced: Manual Device ID
If the mod fails to detect your device, you can manually specify the device ID in `spotify_tokens.ini`:
```ini
DEVICE_ID=<manually_obtained_device_id>
```

To find your device ID:
1. Go to https://developer.spotify.com/dashboard
2. Create an application if you haven't already
3. Use the Web API console to call: `GET https://api.spotify.com/v1/me/player/devices`
4. Find your computer device and copy its ID

## Support

For issues related to:
- **Spotify API**: Visit https://developer.spotify.com/documentation
- **XNFSMusicPlayer**: Check the console for detailed error logs

## Notes

- The mod uses the Spotify Web API, which requires an active internet connection
- Your tokens are stored locally on your computer
- The mod respects your Spotify account settings (region restrictions, etc.)
- Playback will only work on your registered device while this application is running
