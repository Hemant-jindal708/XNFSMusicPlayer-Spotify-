# Spotify Integration Compilation Guide

## Overview
The Spotify integration adds the following new files that must be compiled with the XNFSMusicPlayer project:

### New Files Added
1. **SpotifyController.h** - Header file with Spotify controller class and C-style wrapper functions
2. **SpotifyController.cpp** - Implementation of Spotify controller functionality
3. **SPOTIFY_INTEGRATION.md** - User documentation
4. **COMPILATION_GUIDE.md** - This file

### Modified Files
1. **XNFSMusicPlayer_Common.h** 
   - Added include for SpotifyController.h
   - Added bSpotifyEnabled flag
   - Modified PauseChannelCave() to use Spotify when enabled
   - Modified ResumeChannelCave() to use Spotify when enabled
   - Updated InitConfig() to initialize Spotify

## Dependencies

The Spotify integration requires the following libraries (already in your project):
- **cpr** - HTTP client library for Spotify API calls
- **nlohmann/json** - JSON parsing library
- **httplib** - HTTP server library for OAuth callback handling
- **Windows.h** - Windows API for system functions

These libraries are already included in the Spotify Files directory.

## Build Instructions

### Visual Studio (Recommended)

1. **Add SpotifyController to the project:**
   - Right-click on the project in Solution Explorer
   - Select "Add > Existing Item"
   - Navigate to `XNFSMusicPlayer\SpotifyController.cpp`
   - Repeat for any other new files

2. **Configure Project Properties:**
   - Go to Project Properties
   - C/C++ > General > Additional Include Directories
   - Add the Spotify Files directory path to include the Spotify headers

3. **Link Spotify Implementation:**
   - Ensure SpotifyController.cpp is compiled with the project
   - The linker should automatically link the dependencies

4. **Build the Project:**
   - Build > Rebuild Solution
   - Verify no compilation errors

### Compiler Settings

**Minimum Requirements:**
- C++11 or higher
- /EHsc exception handling
- /Zi debug info (for debugging)

**Recommended Flags:**
```
/W4 /WX /EHsc /Zi /std:c++17
```

## Testing the Build

### Step 1: Verify Compilation
- Ensure `SpotifyController.obj` is generated during compilation
- Check the Output window for any linking errors

### Step 2: Test Spotify Controller Initialization
1. Create a simple test application that calls `SpotifyInit()`
2. Verify it attempts to read tokens from `spotify_tokens.ini`
3. Confirm the Spotify browser login flow triggers if no tokens exist

### Step 3: Integration Test
1. Run the NFS MW 2005 game with the mod enabled
2. Check that:
   - Spotify integration initializes (check console output)
   - Device ID is stored in `spotify_tokens.ini`
   - Play/Pause commands are properly forwarded to Spotify

## Troubleshooting Build Issues

### "SpotifyController.h: No such file or directory"
- Ensure the file path in the #include is correct
- Check that SpotifyController.h is in the XNFSMusicPlayer directory

### Linker Errors (Unresolved External Symbol)
- Verify SpotifyController.cpp is added to the project
- Check that the function declarations match between .h and .cpp

### "cpr not found" or JSON header errors
- Verify the Spotify Files include paths are added to the project
- Check that all dependencies are properly installed

### C2661: no overloaded function takes X arguments
- Ensure you're using the correct C-style wrapper functions (SpotifyInit, SpotifyPlay, etc.)
- Don't call the C++ class methods from C code without proper extern "C" declarations

## Project Structure After Integration

```
XNFSMusicPlayer/
├── SpotifyController.h          (NEW)
├── SpotifyController.cpp        (NEW)
├── XNFSMusicPlayer.h
├── XNFSMusicPlayer_Common.h     (MODIFIED)
├── XNFSMusicPlayer_MW.h
├── XNFSMusicPlayer_Carbon.h
├── XNFSMusicPlayer_TagReader.h
├── XNFSMusicPlayer_StreamPusher.h
├── Carbon_MusicAddresses.h
├── MW_MusicAddresses.h
├── CPatch.h
└── ...

Spotify FIles/
├── main.cpp
├── play.h
├── play.cpp
├── run.bat
└── spotify_tokens.ini
```

## Next Steps

1. **Configure INI file:**
   - Copy `ini-mw/XNFSMusicPlayer_Spotify.ini` to your game's scripts directory
   - Set `EnableSpotify=1` in the [Spotify] section

2. **Test the Integration:**
   - Run the game and verify Spotify commands work
   - Check the log file for any errors

3. **Monitor Console Output:**
   - The mod will print status messages about Spotify initialization
   - Look for messages like "[Spotify] Initialization complete!"

## Advanced: Debugging

To enable detailed debugging:

1. **In XNFSMusicPlayer.ini, set:**
   ```ini
   [XNFSMusicPlayer]
   VerbosityLevel=3
   ```

2. **Watch for these debug messages:**
   - "[Spotify] Token management..."
   - "[Spotify] Device ID: ..."
   - "[Spotify] Sending pause/play command..."

3. **Check the log file:**
   - Default: `scripts\XNFSMusicPlayer\XNFSMusicPlayer.log`

## Performance Considerations

- Spotify API calls add ~50-200ms latency per command
- Ensure your internet connection is stable for reliable playback control
- The mod uses async HTTP requests, so the game UI shouldn't freeze during API calls

## Support

For compilation issues:
- Review the error messages carefully
- Ensure all dependencies are properly installed
- Check that file paths are correct (especially includes)

For runtime issues:
- Check the XNFSMusicPlayer.log file
- Enable verbose logging (VerbosityLevel=3)
- Verify Spotify is running before testing
