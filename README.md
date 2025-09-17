# audio-balance

A zero dependency macOS command-line tool for setting left and right channel volume levels independently. Perfect to adjust your sound system so you're always in the sweet spot of your audio setup, regardless of your exact physical position.

## What is this?

This is a single C file that uses macOS Core Audio APIs to control individual channel volumes. It's vibe-coded and meant to be tweaked to your needs.

## Prerequisites

You need Xcode (or at least Xcode Command Line Tools) installed:
```bash
xcode-select --install
```

## Build

```bash
make
```

The executable will be built in the `out/` directory.

## Usage

```bash
# Set left channel to 75%, right channel to 25%
./out/audio-balance 0.75 0.25

# Only apply if current device matches exactly
./out/audio-balance --if-device "Elgato Dock" 0.75 0.25

# Show version
./out/audio-balance --version
```

Volume values range from 0.0 to 1.0.

## Installation

Think about where you want to put this binary. You could:
- Use `make install` to copy it to `/usr/local/bin/`
- Put it somewhere else in your PATH
- Keep it local and call it directly

## Use Cases

- **Large desk setup**: Got speakers on both ends of your desk? Adjust balance based on where you're sitting
- **macOS Shortcuts**: Perfect for automation - create shortcuts that adjust balance when you switch workspaces
- **Quick audio tweaks**: Fast way to compensate for hearing differences or room acoustics

## Customization

This is a single C file - feel free to:
- Modify the volume ranges
- Add presets for your common configurations
- Change the device detection logic
- Add your own features

The code is straightforward and uses standard Core Audio APIs.