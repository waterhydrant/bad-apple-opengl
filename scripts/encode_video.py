import cv2
import struct
from pathlib import Path

def compress_frame_rle(frame_bytes) -> bytearray:
    rle_data = bytearray()
    if len(frame_bytes) == 0:
        return rle_data

    current_color = 1 if frame_bytes[0] == 255 else 0
    run_length = 0

    for pixel in frame_bytes:
        color = 1 if pixel == 255 else 0
        if color == current_color and run_length < 127:
            run_length += 1
        else:
            packet = (current_color << 7) | run_length
            rle_data.append(packet)

            current_color = color
            run_length = 1

    if run_length > 0:
        packet = (current_color << 7) | run_length
        rle_data.append(packet)

    return rle_data

def mp4_to_rle(input_mp4, output_bin) -> None:
    cap = cv2.VideoCapture(input_mp4)
    if not cap.isOpened():
        print(f"Could not open {input_mp4}")
        return

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

    print(f"Processing {frame_count} frames of {width}x{height}")

    with open(output_bin, "wb") as f:
        f.write(struct.pack("<HHH", width, height, frame_count))

        processed_frames = 0
        while True:
            ret, frame = cap.read()
            if not ret:
                break

            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

            _, binary_frame = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)

            flat_pixels = binary_frame.flatten()

            rle_frame_data = compress_frame_rle(flat_pixels)

            f.write(struct.pack("<H", len(rle_frame_data)))
            f.write(rle_frame_data)

            processed_frames += 1
            if processed_frames % 500 == 0:
                print(f"Compressed {processed_frames}/{frame_count} images")

    cap.release()
    print(f"Successfully generated {output_bin}")

if __name__ == "__main__":
    mp4_to_rle("bad_apple.mp4", "bad_apple.bin")