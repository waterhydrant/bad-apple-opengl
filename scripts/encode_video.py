import struct
from pathlib import Path

import cv2
from moviepy.video.io.VideoFileClip import VideoFileClip
import sys

REPO_ROOT = Path(__file__).resolve().parent.parent

ASSETS_DIR = REPO_ROOT / "assets" / "generated"
MEDIA_DIR = REPO_ROOT / "media"

def print_progress_bar(iteration: int, total: int, bar_length: int = 40) -> None:
    if total <= 0:
        return

    percent = min(100.0, max(0.0, (iteration / total) * 100.0))
    filled_length = (bar_length * iteration) // total
    bar = "█" * filled_length + "░" * (bar_length - filled_length)

    sys.stdout.write(f"\rProgress: |{bar}| {percent:.1f}% ({iteration}/{total} frames)")
    sys.stdout.flush()

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

    print(f"Processing {frame_count} frames of dimensions {width}x{height}")

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
            print_progress_bar(processed_frames, frame_count)

    cap.release()
    print(f"\nSuccessfully generated {output_bin}")


def extract_audio_to_wav(mp4_path, wav_path) -> None:
    video = VideoFileClip(mp4_path)

    video.audio.write_audiofile(wav_path, codec="pcm_s16le")

    video.close()


if __name__ == "__main__":
    print("Compressing video...")
    mp4_to_rle(MEDIA_DIR / "bad_apple.mp4", ASSETS_DIR / "bad_apple_video.bin")
    print("Compression done")
    print("Extracting audio...")
    extract_audio_to_wav(
        MEDIA_DIR / "bad_apple.mp4", ASSETS_DIR / "bad_apple_audio.wav"
    )
    print("All done!")
