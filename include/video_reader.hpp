#pragma once

#include <span>
#include <cstdint>
#include <vector>

/*
 * @brief Reads the RLE-compressed video file and decodes it on the fly
 */
class VideoReader {
  public:
    VideoReader(const char *filePath);
    std::span<std::uint8_t> getFrameData(std::uint16_t frameIndex);

#pragma pack(push, 1)
    struct FileHeader {
        std::uint16_t width;
        std::uint16_t height;
        std::uint16_t frameCount;
    };
#pragma pack(pop)
    FileHeader header;
    std::vector<char> fileData;
    std::vector<std::uint32_t> indexTable;

    std::vector<std::uint8_t> pixelCanvas;
};
