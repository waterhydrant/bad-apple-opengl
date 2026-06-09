#include "video_reader.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

VideoReader::VideoReader(const char *filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: could not open video file " << filePath << '\n';
        std::abort();
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    std::size_t bufferSize =
        fileSize - sizeof(header) - sizeof(std::uint16_t) * header.frameCount;
    fileData.resize(bufferSize);
    indexTable.resize(header.frameCount);
    pixelCanvas.resize(header.width * header.height);

    if (!file) {
        std::cerr << "Error reading file header\n";
        std::abort();
    }

    std::uint32_t currentLoc = 0;
    for (std::uint32_t i = 0; i < header.frameCount; i++) {
        std::uint16_t frameSize;
        file.read(reinterpret_cast<char *>(&frameSize), sizeof(frameSize));

        indexTable[i] = currentLoc;

        file.read(fileData.data() + currentLoc, frameSize);

        currentLoc += frameSize;
    }
}

std::span<std::uint8_t> VideoReader::getFrameData(std::uint16_t frameIndex) {
    std::size_t canvasI = 0;
    std::size_t canvasSize = pixelCanvas.size();

    std::uint32_t index = indexTable[frameIndex];
    std::uint32_t frameSize = (frameIndex >= indexTable.size() - 1)
                                  ? (fileData.size() - index)
                                  : (indexTable[frameIndex + 1] - index);

    for (std::uint32_t i = 0; i < frameSize; i++) {
        std::uint8_t packet = fileData[index + i];

        std::uint8_t color = packet >> 7;
        std::uint8_t length = packet & 0x7F;
        std::uint8_t pixelValue = (color == 1) ? 255 : 0;

        if (canvasI + length > canvasSize) {
            length = canvasSize - canvasI;
        }
        std::fill_n(pixelCanvas.begin() + canvasI, length, pixelValue);
        canvasI += length;
    }

    return pixelCanvas;
}
