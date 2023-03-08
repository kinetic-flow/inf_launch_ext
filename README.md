# infzoom

infzoom is a custom launcher for beatmania IIDX Infinitas.

## Disclaimers

infzoom is based on [darekasan/inf_launch_ext](https://github.com/darekasan/inf_launch_ext).

infzoom will use functionalities built into Windows to change window decorations and resize the game window. None of this is invasive; it's very unlikely that any of this would be considered a violation of Terms of Service / EULA of Infinitas. It explicitly **does not** hook into DirectX or inject any code into the game or its runtime dependencies.

That being said, contributors of this project (or the fork this project is based on) are **not** responsible for any consequences of using this project, which can include (but not limited to) getting banned from the game service.

Use at your own risk.

## Features

[Click here for video demo](https://www.youtube.com/watch?v=Nb6E8KtnKzw)

The launcher provides the following options:

 1. Skipping the updater and launching the game directly
 1. Ability to use ASIO audio output, instead of built-in WASAPI
 1. Windowed mode (resizable)
 1. Ability to pan and zoom in/out, with presets.

It should work with both Japanese and Korean versions. It has been tested primarily with the Japanese version.

#4 is useful for players with smaller monitors, since Infinitas does not have a "wide skin" or a "CS skin".

Tested on Windows 10 and Windows 11.

## Zoom mode: important notes

This application allows the game window to stretch beyond the visible desktop area, like this:

![window size demo](https://raw.githubusercontent.com/kinetic-flow/infzoom/master/doc_img/monitor.png) <br>

... which effectively gives you the "zoom in" function. You can also "zoom out" instead, of course.

A side effect of this is that, if you have multiple monitors, the game window may spill over to other windows.

Running the game in windowed mode is not originally intended to be possible. You may or may not experience sync issues. Ensure you are launching the game with the correct refresh rate. Your timing offset will most likely need to be changed.

The game window is scaled without any filters, so it can look pixelated. There is not much that can be done about that in this tool, since it (purposefully) does not hook into DirectX. You can try to work around this by changing the Windows desktop size to be something smaller (maybe 1280x720) and launching the game, which would let your GPU or your monitor perform the scaling instead.

If you're trying to capture the game with OBS, use Game Capture. It will let you capture the entire screen, regardless of the zoom settings. If you use other capture methods, you won't get the full image.

## Installation

#### 1 Go to [Releases](https://github.com/kinetic-flow/infzoom/releases) page and download & extract the latest version.

#### 2 Open a PowerShell window as administrator

#### 3 Change PowerShell execution policy
```
PS> Set-ExecutionPolicy Bypass

Execution Policy Change
The execution policy helps protect you from scripts that you do not trust. Changing the execution policy might expose
you to the security risks described in the about_Execution_Policies help topic at
https:/go.microsoft.com/fwlink/?LinkID=135170. Do you want to change the execution policy?
[Y] Yes  [A] Yes to All  [N] No  [L] No to All  [S] Suspend  [?] Help (default is "N"):
```
Type Y and press enter. This is required to execute the downloaded PowerShell script.

#### 4 Run the script and install it

Navigate to the folder where you extracted the contents to.

For Japanese version, run inf_launch_ext.ps1. For Korean Infinitas service by USTA, run inf_launch_ext<strong>_kr</strong>.ps1.

```
PS> .\inf_launch_ext.ps1
currently command: "C:\Games\beatmania IIDX INFINITAS\\launcher\modules\bm2dx_launcher.exe" "%1"

script path: C:\Users\darekasan\Downloads\inf_launch_ext.ps1
game path: C:\Games\beatmania IIDX INFINITAS\

0 : revert to default
1 : set to this script path
3 : copy script file to game directory and set to new script path (recommended)
number->:
```

Here, type 3, and press enter; you are done.

Once this is done, when you try to launch Infinitas from the website, it will automatically launch this custom launcher instead of the game.

## Running the game

When you log into Infinitas website in the browser and launch the game, you will be shown the following prompt instead of the game launcher:

```
Executing: inf_launch_ext.ps1
JP eamusement mode
Please select option.
0 : Launcher (required for game updates)
1 : WASAPI
2 : WASAPI + window mode
3 : ASIO
4 : ASIO + window mode
5 : WASAPI + fullscreen borderless with zoom
6 : ASIO + fullscreen borderless with zoom
number(press enter for option 0):
```

### 0 : Launcher

Launches the original Infinitas launcher, which lets you change settings and update the game. You should run this once in a while to check for updates, since other options bypass the update check.

### 1 : WASAPI

Run the game right away, with WASAPI audio (the default audio mode) in full screen.

### 2 : WASAPI + window mode

Same as #2, but launch the game in a resizable window. 

### 3 : ASIO

Run the game right away, with ASIO audio in full screen.

Making the game use ASIO audio requres extra steps - see [instructions from inf_launch_ext](https://github.com/darekasan/inf_launch_ext/blob/master/asio.md) and [this page on iidx.org](https://iidx.org/misc/infinitas_asio).

### 4 : ASIO + window mode

Same as #3, but launch the game in a resizable window. 

### 5 : WASAPI + fullscreen borderless with zoom

Launch the game with WASAPI audio with "borderless fullscreen" window. It also runs infzoom.exe which give you the ability to resize the window with hotkeys.

To change settings, like which monitor is used, modify the zoom presets, or to change the hotkeys: modify **infzoom.ini** in the game directory.

The default hotkeys are:

* Zoom into 1P: **F1**
* Zoom into 2P: **F2**
* Zoom into DP: **F3**
* Reset zoom & enable manual mode: **F5**
* In manual mode, move window: **ctrl + up, down, left, or right**
* In manual mode, change zoom: **alt + up, down, left, or right**
* Forcibly close game: **alt + F10**

### 6 : ASIO + fullscreen borderless with zoom

Same as #5, but with ASIO audio.

## Troubleshooting

Look for infzoom.log in the game directory.

## Uninstallation

It is not sufficient to delete the PowerShell script. Instead, run the script again, but choose option "0 : revert to default". After that, you can delete the script.
