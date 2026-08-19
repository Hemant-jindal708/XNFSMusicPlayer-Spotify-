# Xan's NFS Music Player + Spotify Integration

This is a custom music player replacement and a BASS library interface for Need for Speed: Most Wanted (2005), now featuring full Spotify support! 

> [!IMPORTANT]
> **Spotify Premium Recommendation**: It is highly recommended to have a **Spotify Premium** account to use this mod seamlessly. Free accounts may face limitations with API controls, playback, and token management.

## FEATURES

### Base Features
- Custom playlist generation - put any number of songs you like!
- Basic support for M3U playlists!
- Interactive music playback - you're not missing out on these! You can even modify them to your liking! (requires a bit of skill)
- Online streaming - listen to online radio in the game!
- Automatic metadata reading
- SHOUTcast metadata support - a track changed on the radio? No problem, the Chyron will pop up when it happens!
- Music looping
- Per track settings - need to change the song's shown title or change the loop point? You can set that in a separate settings file!
- Game mastered audio - the game itself controls the music volume, just like its own music player

### Spotify Features ✨ NEW
- **Stream Spotify Music** - Play any Spotify track in-game instead of local MP3s
- **Full Playback Control** - Play/pause from game controls  
- **Track Navigation** - Skip next/previous with media keys
- **Real-time Metadata Display** - See artist, track, album in game UI
- **OAuth2 Authentication** - Secure browser-based login
- **Automatic Token Management** - Tokens refresh automatically
- **Device Auto-Detection** - Works seamlessly with your computer

## INSTALLATION

1. Download the latest release `.zip` file from the [Releases page](../../releases).
2. Install Visual Studio 2015-2022 x86 Redist (from here: https://aka.ms/vs/17/release/vc_redist.x86.exe) - **Skip if already installed**.
3. Extract the downloaded `.zip` archive.
4. Copy **all extracted files and folders** and paste them directly into your game's main directory (the folder containing `speed.exe`).
5. Open `scripts\XNFSMusicPlayer.ini` and ensure `EnableSpotify=1` under the `[Spotify]` section.

## HOW TO USE SPOTIFY STREAMING

To ensure a smooth experience, please follow these steps before launching the game:

1. **Pre-open Spotify**: Open the Spotify desktop app or web player on your computer and start playing any song. This establishes an active playback device for the mod to connect to.
2. **Launch the Game**: Start Need for Speed: Most Wanted.
3. **Authentication (First Run Only)**: A browser window will open asking you to log in to Spotify and grant permissions to the mod. Click "Approve".
4. Return to the game, and the mod will automatically connect to your active Spotify session.

### CONTROLS

You can control Spotify playback directly while driving using your keyboard's media keys:

- **Next Song**: Press the **Media Next Key** (often `Fn + Right Arrow` or a dedicated Next Track button).
- **Previous Song**: Press the **Media Previous Key** (often `Fn + Left Arrow` or a dedicated Prev Track button).
- **Play/Pause**: Handled automatically by the game's pause menu, or using your keyboard's Play/Pause media key.

Whenever a track changes, the in-game UI (Chyron) will instantly display the song and artist name!

## CONFIGURATION

The configuration consists of multiple files located in the `scripts\XNFSMusicPlayer\` directory:

1. **XNFSMusicPlayer.ini**: The main configuration file.
2. **spotify_tokens.ini**: Your automatically generated Spotify login tokens. (Delete this file if you need to re-authenticate).
3. **Playlist.m3u**: The main playlist for local MP3 playback (if Spotify is disabled).
4. **PlaylistConfig.ini**: Per-track settings.

### Enable/Disable Spotify
In `scripts\XNFSMusicPlayer.ini` (or `scripts\XNFSMusicPlayer\XNFSMusicPlayer.ini`), you can toggle Spotify support:
```ini
[Spotify]
EnableSpotify=1              ; Set to 1 to enable Spotify, 0 to disable and use local MP3s
```

### Debug Logging
If you encounter issues with Spotify, enable verbose logging to see errors in the console:
```ini
[XNFSMusicPlayer]
VerbosityLevel=3   ; Shows all [Spotify] messages in console
```

## TROUBLESHOOTING

- **Spotify initialization failed / No track info displayed**: Make sure you have the Spotify app open and playing a song *before* starting the game. Ensure `EnableSpotify=1` in the configuration.
- **Tokens keep getting deleted**: Your authorization may have expired or failed. Verify your Spotify Premium subscription and try restarting the game to re-authenticate.
- **Hotkeys not working**: Ensure your keyboard media keys are working properly in Windows. 

## TODO list:
- add NFSC chyron handling and calling
- add NFSC music playback types like MW - IG, FE, AL, OF (instead of just on/off)
- Unicode
- installer direct process calling instead of batch scripting
- sflist support
- proper versioning generation & Git integration
- integrate BASS FX to restore the LPF effect on SpeedBreaker

## CREDITS

- ThirteenAG - for the [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) and the injector and IniReader
- [Un4seen Developments Ltd.](http://www.un4seen.com/) (Ian Luck) - BASS library, BASSMIDI and BASSFLAC addons
