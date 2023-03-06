# infzoom

infzoom is a custom launcher for beatmania IIDX Infinitas.

## Disclaimers

infzoom is based on [darekasan/inf_launch_ext](https://github.com/darekasan/inf_launch_ext).

infzoom will use functionalities built into Windows to change window decorations and resize the game window. None of this is invasive and very unlikely that any of this would be considered a violation of Terms of Service / EULA of Infinitas.

That being said, contributors of this project (or the fork this project is based on) are **not** responsible for any consequences of using this project, which can include (but not limited to) getting banned from the game service.

Use at your own risk.

## Features

The launcher provides the following options:

 1. Skipping the updater and launching the game directly
 1. Ability to use ASIO audio output, instead of built-in WASAPI
 1. Windowed mode (resizable)
 1. Ability to pan and zoom in/out, with presets.

It should work with both Japanese and Korean versions. It has been tested primarily with the Japanese version.

## Installation

#### 1 Go to [Releases](https://github.com/kinetic-flow/infzoom/releases) page and download the latest version.

#### 2 Open a PowerShell window as administrator

#### 3 Change PowerShell execution policy
```
PS> Set-ExecutionPolicy Bypass
```
This is required to execute the downloaded PowerShell script.

#### 4 Run the script and install it

For Japanese version, run inf_launch_ext.ps1.

For Korean Infinitas service by USTA, run inf_launch_ext_kr.ps1.

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

Run the game right away, with WASAPI audio (the default audio mode).

### 2 : WASAPI + window mode

Same as #2, but launch the game in a resizable window. 

### 3 : ASIO

Run the game right away, with ASIO audio.

Making the game use ASIO audio requres extra steps - see [this link](https://github.com/darekasan/inf_launch_ext/blob/master/asio.md).

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
* Forcibly close game: **alt+ F10****

### 6 :ASIO + fullscreen borderless with zoom

Same as #5, but with ASIO audio.

## Zoom mode: important notes

This application allows the game window to stretch beyond the visible desktop area, like this:

![window size demo](https://raw.githubusercontent.com/kinetic-flow/infzoom/master/doc_img/monitor.png) <br>

... which effectively gives you the "zoom in" function.

A side effect of this is that, if you have multiple monitors, the game window may spill over to other windows.

Running the game in windowed mode is not originally intended to be possible. You may or may not experience sync issues. Ensure you are launching the game with the correct refresh rate.

The game window is scaled without any filters, so it can look pixelated. There is not much that can be done about that, but you can work around this by changing the Windows desktop size to be something smaller (maybe 1280x720) and launching the game.

## Troubleshooting

Look for infzoom.log in the game directory.

## Uninstallation

It is not sufficient to delete the PowerShell script. Instead, run the script again, but choose option "0 : revert to default". After that, you can delete the script.
