#include "WorkerBridge.h"
#include <stdio.h>
#include "Logger.h"

namespace
{
    HANDLE g_liveViewPipe = INVALID_HANDLE_VALUE;

    const char* PipeName =
        R"(\\.\pipe\WeddingBooth.LiveView)";
}

BOOL InitializeLiveViewPipe()
{
    while (true)
    {
        g_liveViewPipe = CreateFileA(
            PipeName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (g_liveViewPipe != INVALID_HANDLE_VALUE)
        {
            WriteLog("Connected to ASP.NET pipe");
            printf("Connected to ASP.NET LiveView pipe.\n");
            return TRUE;
        }

        printf("Waiting for ASP.NET LiveView pipe...\n");
        Sleep(1000);
    }
}

BOOL PublishLiveViewFrame(
    const unsigned char* jpegData,
    ULONG jpegLength
)
{
    if (
        g_liveViewPipe == INVALID_HANDLE_VALUE ||
        jpegData == NULL ||
        jpegLength == 0
    )
    {
        return FALSE;
    }

    DWORD bytesWritten = 0;

    // 先傳 4 bytes frame length
    if (!WriteFile(
        g_liveViewPipe,
        &jpegLength,
        sizeof(jpegLength),
        &bytesWritten,
        NULL
    ))
    {
        WriteLog("Failed to write frame length. Error=%lu", GetLastError());
        CloseLiveViewPipe();

        return FALSE;
    }

    // 再傳 JPEG bytes
    if (!WriteFile(
        g_liveViewPipe,
        jpegData,
        jpegLength,
        &bytesWritten,
        NULL
    ))
    {
        WriteLog("Failed to write JPEG frame. Error=%lu", GetLastError());
        CloseLiveViewPipe();

        return FALSE;
    }

    return bytesWritten == jpegLength;
}

BOOL IsLiveViewPipeConnected()
{
    return g_liveViewPipe != INVALID_HANDLE_VALUE;
}

void CloseLiveViewPipe()
{
    if (g_liveViewPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_liveViewPipe);
        g_liveViewPipe = INVALID_HANDLE_VALUE;
    }
}

BOOL TryReadWorkerCommand(
    char* commandBuffer,
    DWORD bufferLength
)
{
    if (
        g_liveViewPipe == INVALID_HANDLE_VALUE ||
        commandBuffer == NULL ||
        bufferLength == 0
    )
    {
        return FALSE;
    }

    DWORD availableBytes = 0;

    if (!PeekNamedPipe(
        g_liveViewPipe,
        NULL,
        0,
        NULL,
        &availableBytes,
        NULL
    ))
    {
        return FALSE;
    }

    if (availableBytes == 0)
    {
        return FALSE;
    }

    DWORD bytesRead = 0;

    const DWORD maxRead =
        bufferLength - 1;

    if (!ReadFile(
        g_liveViewPipe,
        commandBuffer,
        maxRead,
        &bytesRead,
        NULL
    ))
    {
        return FALSE;
    }

    commandBuffer[bytesRead] = '\0';

    return TRUE;
}

BOOL SendWorkerMessage(
    const char* message
)
{
    if (
        g_liveViewPipe == INVALID_HANDLE_VALUE ||
        message == NULL
    )
    {
        return FALSE;
    }

    DWORD bytesWritten = 0;

    const DWORD length =
        static_cast<DWORD>(
            strlen(message)
        );

    return WriteFile(
        g_liveViewPipe,
        message,
        length,
        &bytesWritten,
        NULL
    );
}