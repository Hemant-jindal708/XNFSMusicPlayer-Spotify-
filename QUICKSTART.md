# Spotify Mod Quick Start Guide

## 🚀 5-Minute Setup

### Prerequisites
- Visual Studio 2015 or later (with C++ tools)
- NFS MW 2005 game
- Spotify account
- Active internet connection

---

## Step 1: Compile the Mod (5 minutes)

1. **Open the project** in Visual Studio
2. **Add new files** to XNFSMusicPlayer project:
   - Right-click project → Add → Existing Item
   - Add `SpotifyController.h`
   - Add `SpotifyController.cpp`

3. **Rebuild solution**:
   - Build → Rebuild Solution
   - Check output: Should compile without errors

4. **Verify**:
   - No red error squiggles
   - Output shows successful build

---

## Step 2: Configure INI File (1 minute)

1. **Find** `scripts/XNFSMusicPlayer/XNFSMusicPlayer.ini`
2. **Add this section** at the bottom:
   ```ini
   [Spotify]
   EnableSpotify=1
   ```

3. **Save the file**

---

## Step 3: Run the Game (1 minute setup + OAuth)

1. **Launch NFS MW 2005** with the mod enabled
2. **First run only**: A browser window will open for Spotify login
   - Log in to your Spotify account
   - Grant permissions when prompted
   - Click "Approve" if needed
3. **Game continues**: Mod auto-completes setup
4. **Check console**: Should show:
   ```
   [Spotify] Spotify integration enabled. Initializing...
   [Spotify] Initialization complete!
   [Spotify] Device ID: <ID>
   ```

---

## Step 4: Test Features (2 minutes)

### Test Play/Pause
- Start a song in Spotify (or game pause button)
- Click game pause button → Should pause on Spotify
- Click game play button → Should resume on Spotify

### Test Next/Previous
- Press **Media Next Key** (or Fn+→) → Spotify skips track
- Press **Media Previous Key** (or Fn+←) → Spotify goes back
- Check console for confirmation

### Test UI Display
- Watch the game chyron (text at top/bottom)
- Should show: `[Spotify] Artist Name - Song Name`
- Updates every ~5 seconds automatically

---

## ✅ Success Indicators

You'll know it's working if you see:

1. **Console Messages**:
   ```
   [Spotify] Initialization complete!
   [Spotify] Device ID: xxxxx
   [Spotify] Track Updated: Artist - Song
   [Spotify] Displaying Spotify Chyron: [Spotify] Artist - Song
   ```

2. **Game UI**:
   - "[Spotify]" text visible in chyron
   - Track name and artist displayed
   - Updates when you skip tracks

3. **Playback Control**:
   - Game pause/play buttons work
   - Media keys (next/prev) work
   - Spotify updates on your phone/other devices in real-time

---

## 🔧 Troubleshooting

### Compilation Errors
**Error**: `SpotifyController.h: No such file or directory`
- **Fix**: Ensure files are in `XNFSMusicPlayer\` directory

**Error**: `Unresolved external symbol`
- **Fix**: Make sure `SpotifyController.cpp` is added to project

### Runtime Issues
**Problem**: "Spotify initialization failed"
- **Fix**: 
  - Ensure Spotify is running before game starts
  - Delete `spotify_tokens.ini` and restart (re-authenticate)

**Problem**: UI not showing track info
- **Fix**: 
  - Check EnableSpotify=1 in INI
  - Check console for error messages
  - Verify Spotify is playing a track

**Problem**: Hotkeys not working
- **Fix**: 
  - Make sure your keyboard has media keys
  - Try using system media key shortcut
  - Check console for "[Spotify] next track command sent"

---

## 📁 Files Created/Modified

**Files You Need To Add To Project:**
- ✅ `XNFSMusicPlayer/SpotifyController.h`
- ✅ `XNFSMusicPlayer/SpotifyController.cpp`

**Files Automatically Modified:**
- `XNFSMusicPlayer/XNFSMusicPlayer_Common.h`
- `XNFSMusicPlayer/XNFSMusicPlayer_MW.h`

**Files You Need To Reference:**
- `SPOTIFY_INTEGRATION.md` - Setup guide
- `COMPILATION_GUIDE.md` - Build instructions
- `SPOTIFY_UI_GUIDE.md` - UI system details
- `IMPLEMENTATION_SUMMARY.md` - Complete documentation

---

## 🎮 What You Can Now Do

### ✅ Playback Control
- Play/pause songs via game buttons
- Works just like local MP3 playback
- Automatically pauses when game pauses

### ✅ Track Navigation
- Skip next with Media Next Key
- Go back with Media Previous Key
- See current track info in game UI
- Updates automatically

### ✅ Real-Time UI Display
- Song name displayed in chyron
- Artist name shown
- Album name shown
- Updates every 5 seconds
- Immediate update on skip

### ✅ Automatic Features
- Token refresh (no re-login needed)
- Device auto-detection
- Fallback to local MP3s if Spotify unavailable
- Error recovery

---

## 🔐 First Time Login Flow

1. **Game starts** with EnableSpotify=1
2. **Browser opens** to Spotify login page
3. **You log in** with your Spotify credentials
4. **Permissions prompt** asks for these permissions:
   - Read currently playing track
   - Control playback
   - See all playlists
5. **You click "Agree"**
6. **Game continues** and auto-completes setup
7. **`spotify_tokens.ini` created** with your tokens
8. **Next time**: No login needed, mod loads tokens automatically

---

## 💡 Tips

1. **Multiple Devices**: Make sure your computer is set as the Spotify device
2. **Offline Mode**: Mod won't work without internet (Spotify API only)
3. **Token Expiration**: Handled automatically - no action needed
4. **Device Switching**: Change in Spotify settings, restart game to re-detect
5. **Hotkeys Not Working?**: Some keyboard brands require special drivers

---

## 🆘 Still Having Issues?

### Enable Debug Logging
In `XNFSMusicPlayer.ini`:
```ini
[XNFSMusicPlayer]
VerbosityLevel=3
```

Check console output for:
- `[Spotify]` messages for integration status
- `[Auth]` messages for authentication
- `[HTTP]` messages for API calls

### Check `spotify_tokens.ini`
Located in game directory, should contain:
```
ACCESS=<token>
REFRESH=<token>
DEVICE_ID=<id>
```

### Manual Device ID
If auto-detection fails, you can manually set:
1. Visit: https://developer.spotify.com/dashboard
2. Create an app if needed
3. Use Web API console to call: `GET /v1/me/player/devices`
4. Copy your device ID
5. Add to `spotify_tokens.ini`: `DEVICE_ID=<your_id>`

---

## 📊 Keyboard Shortcuts Reference

| Action | Shortcut | Works |
|--------|----------|-------|
| **Play** | Game play button | ✅ |
| **Pause** | Game pause button | ✅ |
| **Next** | Media Next (Fn+→) | ✅ |
| **Previous** | Media Prev (Fn+←) | ✅ |
| **Volume** | System volume keys | ✅ |

---

## 🎯 One-Minute Summary

```
1. Add SpotifyController.h/cpp to project
2. Build project
3. Set EnableSpotify=1 in INI
4. Run game
5. Log in to Spotify (first time only)
6. Play/pause/skip via game controls
7. See track info in game UI
```

That's it! 🎉

---

## 📞 Support Resources

- **Spotify API Docs**: https://developer.spotify.com/documentation
- **Game Logs**: Check `scripts/XNFSMusicPlayer/XNFSMusicPlayer.log`
- **Console Output**: Watch VS Output window for debug messages
- **INI Documentation**: See `SPOTIFY_INTEGRATION.md`

---

## Version Info

- **Mod Version**: 1.12.x with Spotify Integration
- **Spotify API**: v1 (Rest API)
- **Tested On**: NFS MW 2005 (2005 release)
- **Requires**: C++11 or higher

---

Happy modding! 🎮🎵
