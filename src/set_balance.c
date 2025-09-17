#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioServices.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.1.0"

void printUsage(const char* programName) {
    printf("Usage: %s [--if-device \"device_name\"] <left_volume> <right_volume>\n", programName);
    printf("  --if-device:  Only set balance if current device matches exactly\n");
    printf("  left_volume:  Volume for left channel (0.0-1.0)\n");
    printf("  right_volume: Volume for right channel (0.0-1.0)\n");
    printf("Examples:\n");
    printf("  %s 0.75 0.25  (75%% left, 25%% right)\n", programName);
    printf("  %s --if-device \"Elgato Dock\" 0.75 0.25\n", programName);
}

void printDeviceName(AudioDeviceID deviceID) {
    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyDeviceNameCFString,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };

    CFStringRef deviceName;
    UInt32 size = sizeof(deviceName);

    OSStatus status = AudioObjectGetPropertyData(deviceID, &propertyAddress, 0, NULL, &size, &deviceName);
    if (status == noErr && deviceName) {
        char name[256];
        Boolean success = CFStringGetCString(deviceName, name, sizeof(name), kCFStringEncodingUTF8);
        if (success) {
            printf("Current output device: %s\n", name);
        }
        CFRelease(deviceName);
    } else {
        printf("Current output device: Unknown\n");
    }
}

int getDeviceName(AudioDeviceID deviceID, char* buffer, size_t bufferSize) {
    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyDeviceNameCFString,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };

    CFStringRef deviceName;
    UInt32 size = sizeof(deviceName);

    OSStatus status = AudioObjectGetPropertyData(deviceID, &propertyAddress, 0, NULL, &size, &deviceName);
    if (status == noErr && deviceName) {
        Boolean success = CFStringGetCString(deviceName, buffer, bufferSize, kCFStringEncodingUTF8);
        CFRelease(deviceName);
        return success ? 0 : 1;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    char* requiredDevice = NULL;
    int volumeArgStart = 1;

    // Parse flags
    if (argc >= 2 && strcmp(argv[1], "--version") == 0) {
        printf("%s\n", VERSION);
        return 0;
    }

    if (argc >= 4 && strcmp(argv[1], "--if-device") == 0) {
        requiredDevice = argv[2];
        volumeArgStart = 3;
    }

    // Check for correct number of arguments
    if ((requiredDevice && argc != 5) || (!requiredDevice && argc != 3)) {
        printUsage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    Float32 leftVolume = atof(argv[volumeArgStart]);
    Float32 rightVolume = atof(argv[volumeArgStart + 1]);

    // Validate input values
    if (leftVolume < 0.0 || leftVolume > 1.0 || rightVolume < 0.0 || rightVolume > 1.0) {
        printf("Error: Volume values must be between 0.0 and 1.0\n");
        printUsage(argv[0]);
        return 1;
    }

    AudioDeviceID outputDevice;
    UInt32 size = sizeof(outputDevice);
    OSStatus status;

    // Get default output device
    AudioObjectPropertyAddress propertyAddress = {
        kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain  // Updated from deprecated kAudioObjectPropertyElementMaster
    };

    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &size, &outputDevice);
    if (status != noErr) {
        printf("Error getting default output device: %d\n", status);
        return 1;
    }


    // Check if device name matches requirement
    if (requiredDevice) {
        char currentDeviceName[256];
        if (getDeviceName(outputDevice, currentDeviceName, sizeof(currentDeviceName)) != 0) {
            printf("Error: Could not get current device name\n");
            return 1;
        }

        if (strcmp(currentDeviceName, requiredDevice) != 0) {
            return 0;
        }
    }

    // Check if device supports volume control
    propertyAddress.mSelector = kAudioDevicePropertyVolumeScalar;
    propertyAddress.mScope = kAudioDevicePropertyScopeOutput;
    propertyAddress.mElement = 1; // Left channel

    Boolean hasProperty = AudioObjectHasProperty(outputDevice, &propertyAddress);
    if (!hasProperty) {
        printf("Device doesn't support individual channel volume control\n");
        return 1;
    }
    
    // Set left channel volume
    size = sizeof(Float32);
    status = AudioObjectSetPropertyData(outputDevice, &propertyAddress, 0, NULL, size, &leftVolume);
    if (status != noErr) {
        printf("Error setting left channel volume: %d\n", status);
    }
    
    // Set right channel volume  
    propertyAddress.mElement = 2; // Right channel
    status = AudioObjectSetPropertyData(outputDevice, &propertyAddress, 0, NULL, size, &rightVolume);
    if (status != noErr) {
        printf("Error setting right channel volume: %d\n", status);
    }
    
    return 0;
}
