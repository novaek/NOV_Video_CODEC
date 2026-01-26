#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

int main() {
    int length = 0;
    std::cin >> length;

    std::vector<uint8_t> bytes;
    bytes.reserve(length);

    for (int i = 0; i < length; ++i) {
        unsigned int value;
        std::cin >> std::hex >> value;   // 👈 read hex
        bytes.push_back(static_cast<uint8_t>(value));
    }

    std::ofstream out("KGB", std::ios::binary);
    if (!out) {
        std::cerr << "file not opened\n";
        return 1;
    }

    out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    out.close();
}
