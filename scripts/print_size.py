import os

file = "bad_apple.bin"

file_size = os.path.getsize(file)
print(f"File size: {file_size}")