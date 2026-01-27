#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


#pragma comment(lib, "User32.lib")

std::vector<uint16_t> framebuffer;
int width =0;
int height = 0;
std::vector<uint8_t> rgb;


void rgb565_to_rgb24(const std::vector<uint16_t>& src, std::vector<uint8_t>& dst, int width,int height)
{
    dst.resize(width * height * 3);

    for (int i = 0; i < width * height; i++) {
        uint16_t p = src[i];

        uint8_t r = ((p >> 11) & 0x1F) << 3;
        uint8_t g = ((p >> 5)  & 0x3F) << 2;
        uint8_t b = (p & 0x1F) << 3;

        dst[i * 3 + 0] = b;
        dst[i * 3 + 1] = g;
        dst[i * 3 + 2] = r;
    }
}

void render_frame(HWND hwnd, const std::vector<uint8_t>& rgb, int width, int height)
{
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(hwnd);

    StretchDIBits(hdc,0, 0, width, height, 0, 0, width, height, rgb.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);

    ReleaseDC(hwnd, hdc);
}



uint16_t read_u16(const std::vector<uint8_t>& data, size_t& offset) {
    uint16_t v = (data[offset] << 8) | data[offset + 1];
    offset += 2;
    return v;
}

void codec(std::vector<uint8_t>& data, std::vector<uint16_t>& framebuffer){
    size_t offset = 0;

    uint8_t frame_type = data[offset++];
    width  = read_u16(data, offset);
    height = read_u16(data, offset);

    size_t pixel_count = width * height;

    if (frame_type == 0x00) {
        framebuffer.resize(pixel_count);
        for (size_t i = 0; i < pixel_count; i++) {
            framebuffer[i] = read_u16(data, offset);
        }
        return;
    }

    if (frame_type == 0x01) {

        size_t cursor = 0;
        uint16_t chunk_count = read_u16(data, offset);

        for (int c = 0; c < chunk_count; c++) {
            if (offset +1> data.size()) {
                std::cerr << "Frame data corrupted\n";
                return;
            }
            uint16_t skip = read_u16(data, offset);
            cursor += skip;
            uint16_t change = read_u16(data, offset);

            for (int i = 0; i < change; i++) {
                uint16_t delta = read_u16(data, offset);
                if (cursor >= framebuffer.size()) {
                    std::cerr << "Frame decode overflow\n";
                    return;
                }
                framebuffer[cursor] ^= delta;
                cursor++;
            }
        }
    }
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
   const HWND hDesktop = GetDesktopWindow();
   GetWindowRect(hDesktop, &desktop);
   horizontal = desktop.right;
   vertical = desktop.bottom;
}

std::vector<uint8_t> get_info(std::string name){
    std::vector<uint8_t> info;
    std::ifstream input(name, std::ios::binary);
    if (!input){
        std::cout << "file not shown" << std::endl;
        return info;
    }
    std::vector<uint8_t> bytes(
         (std::istreambuf_iterator<char>(input)),
         (std::istreambuf_iterator<char>()));

    if (bytes.size() < 4) {
        std::cout << "File too small!" << std::endl;
        return info;
    }

    uint16_t header = (bytes[0] << 8) | bytes[1];
    std::cout << std::hex << header << std::endl;
    if (header==0xadda){
        info.push_back(0xFF);
        std::cout << "file header correct" << std::endl;
    }
    else {
        info.push_back(0x00);
        std::cout << "file header incorrect" << std::endl;
    }

    uint16_t lenght=0;
    lenght = (uint16_t(bytes[2]) << 8) | (uint16_t(bytes[3])); 
    std::cout << "\n\n" <<std::hex << static_cast<int>(info[0]) << std::endl;
    std::cout << "\n size : " << std::dec << static_cast<int>(lenght) << std::endl;
    int hor=0;
    int ver = 0;
    GetDesktopResolution(hor,ver);
    uint64_t screenPixels = uint64_t(hor) * ver;
    std::cout << std::dec <<"screen size (pixels): "<< screenPixels << std::endl;
    uint8_t flag =0x40;
    std::cout << "flag : " << flag << " :: " << unsigned(flag) << std::endl;
    int flag_number=0;
    for (int i=0;i<bytes.size();i++){
        if (bytes[i]==flag){
            flag_number+=1;
        }
    }
    std::cout << "number of flag iterations : " << flag_number << std::endl;
    std::cout << "number of possible images : " << flag_number-2 << std::endl;
    input.close();
    return info;

} 
std::vector<int> get_image_index(std::string file){
    std::vector<int> index={};
    std::ifstream input(file, std::ios::binary);
    if (!input){
        std::cout << "file not shown" << std::endl;
        return {-2};
    }
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(input)),(std::istreambuf_iterator<char>()));
    for (int i=0; i<bytes.size(); i++){
        if (bytes[i]==0x40){
            index.push_back(i);
        }
    }
    index.erase(index.begin());
    index.erase(index.end()-1);
    for (int z=0; z<index.size(); z++) {
        std::cout << index[z] << " " << std::endl;
    }
    return index;

}

std::vector<uint8_t> get_img_data(std::vector<int> index, int img, std::string file){
    std::ifstream input(file, std::ios::binary);
    if (!input){
        std::cout << "file not shown" << std::endl;
        return {0x00};
    }
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(input)),(std::istreambuf_iterator<char>()));
    int first_index = index[img]+1;
    int last_index = index[img+1]-1;
    std::vector<uint8_t> image={}; 
    for (int data=first_index;data<last_index; data++)
    {
        image.push_back(bytes[data]);
    }
    return image;
}

int main() {
    get_info("KGB");
    std::vector<int> mp = get_image_index("KGB");
    int size = mp.size();
    int h=0;
    HWND hwnd =GetConsoleWindow();
    while (size>0){
        std::vector<uint8_t> mp2 = get_img_data(mp, h,"KGB");
        codec(mp2, framebuffer);
        rgb565_to_rgb24(framebuffer,rgb, width, height);
        render_frame(hwnd, rgb, width, height);
        h+=1;
        size-=1;
        Sleep(1000/24);
    }
    h=0;
}
