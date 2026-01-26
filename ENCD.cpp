#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


#pragma comment(lib, "User32.lib")

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
    std::vector<uint8_t> mp2 = get_img_data(mp, 1,"KGB");

}