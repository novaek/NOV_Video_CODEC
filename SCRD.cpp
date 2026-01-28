#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

      

constexpr uint16_t MAX_FRAMES =0x0FF;
constexpr uint16_t FPS        = 24;


void deltaEncode(uint8_t* cur, uint8_t* prev, uint8_t* out, int size) {
    for (int i = 0; i < size; i++) {
        out[i] = cur[i] ^ prev[i];
    }
}

int main() {

    char bytes[2];
    uint16_t MAGIC = 0xADDA;
    bytes[0] = (MAGIC >> 8) & 0xFF; 
    bytes[1] = MAGIC & 0xFF;  
    std::cout << "Recording...\n";

    int width  = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    int frameSize = width * height * 2; 


    HDC hScreenDC = GetDC(nullptr);
    HDC hMemDC    = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemDC, hBitmap);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height; 
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 16;
    bmi.bmiHeader.biCompression = BI_BITFIELDS;

    DWORD masks[3] = { 0xF800, 0x07E0, 0x001F };
    memcpy(bmi.bmiColors, masks, sizeof(masks));

    uint8_t* frame565  = new uint8_t[frameSize];
    uint8_t* prevFrame = new uint8_t[frameSize];
    uint8_t* deltaBuf  = new uint8_t[frameSize];
    memset(prevFrame, 0, frameSize);


    std::string mshp="";
    std::cin >> mshp;
    std::ofstream out(mshp, std::ios::binary);
    if (!out) {
        std::cerr << "File open failed\n";
        return 1;
    }


    out.write(bytes, 2);
    out.write((char*)&MAX_FRAMES, sizeof(MAX_FRAMES));

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    auto now = [&]() {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return double(t.QuadPart) / double(freq.QuadPart);
    };

    double frameTime = 1.0 / FPS;
    double nextFrame = now();

    for (uint16_t f = 0; f < MAX_FRAMES; f++) {


        double t = now();
        if (t < nextFrame) {
            Sleep((DWORD)((nextFrame - t) * 1000.0));
        }
        nextFrame += frameTime;

        BitBlt(
            hMemDC, 0, 0, width, height,
            hScreenDC, 0, 0,
            SRCCOPY | CAPTUREBLT
        );

        GetDIBits(
            hMemDC,
            hBitmap,
            0,
            height,
            frame565,
            &bmi,
            DIB_RGB_COLORS
        );


        uint32_t writeSize = frameSize;
        uint32_t size = ((writeSize & 0x000000FF) << 24) | ((writeSize & 0x0000FF00) << 8)  | ((writeSize & 0x00FF0000) >> 8)  | ((writeSize & 0xFF000000) >> 24);

        if (f == 0) {
            out.write((char*)&size, sizeof(size));
            out.write((char*)frame565, frameSize);
        } else {
            deltaEncode(frame565, prevFrame, deltaBuf, frameSize);
            out.write((char*)&size, sizeof(size));
            out.write((char*)deltaBuf, frameSize);
        }

        memcpy(prevFrame, frame565, frameSize);

        if ((f & 0xFF) == 0)
            std::cout << "Frame " << f << "\n";
    }

    out.close();

    delete[] frame565;
    delete[] prevFrame;
    delete[] deltaBuf;

    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreenDC);

    std::cout << "Done.\n";
    return 0;
}
