import struct

def verify_binary() -> None:
    with open("bad_apple.bin", "rb") as f:
        width, height, total_frames = struct.unpack("<HHH", f.read(6))
        print(f"Header info: {width: }, {height: }, {total_frames: }")

        for i in range(100):
            frame_size = struct.unpack("<H", f.read(2))[0]
            first_frame_rle = f.read(frame_size)

        for packet in first_frame_rle[:20]:
            color = "white" if (packet >> 7) == 1 else "black"
            length = packet & 0x7F
            print(f"{color:}, {length:} pixels")

if __name__ == "__main__":
    verify_binary()