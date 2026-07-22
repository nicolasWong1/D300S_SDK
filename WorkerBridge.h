#pragma once

#include <Windows.h>

BOOL InitializeLiveViewPipe();
BOOL PublishLiveViewFrame(
    const unsigned char* jpegData,
    ULONG jpegLength
);
BOOL TryReadWorkerCommand(
    char* commandBuffer,
    DWORD bufferLength
);
BOOL SendWorkerMessage(
    const char* message
);
void CloseLiveViewPipe();