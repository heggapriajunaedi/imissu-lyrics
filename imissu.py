import os
import sys
import time
import random
import unicodedata

# Lirik lagu beserta durasinya
# PERBAIKAN: Baris terakhir sudah ditambah durasi (4.5 detik) agar tidak error
LYRICS = [
    ("So, what if I call", 4),
    ("And you pick up the phone?", 5),
    ("And I use this holiday to make my way to your ghost", 9),
    ("Oh, what if you're lonely", 5),
    ("And you know I am too", 4),
    ("And i get the chance to stay", 3),
    ("Merry Christmas, I miss you", 5), 
]

# Set karakter Unicode estetik pilihanmu
FLAKES = ["⋆", "꙳", "•", "❅", "‧", "❆", "₊"]
WIDTH = 70
HEIGHT = 22
FPS = 0.04

def clear():
    os.system("cls" if os.name == "nt" else "clear")

def hide_cursor():
    sys.stdout.write("\033[?25l")
    sys.stdout.flush()

def show_cursor():
    sys.stdout.write("\033[?25h")
    sys.stdout.flush()

def get_padding():
    try:
        term_height = os.get_terminal_size().lines
        return max(0, (term_height - HEIGHT) // 2)
    except OSError:
        return 0

def get_visual_length(text):
    """Menghitung panjang visual teks, memperhitungkan karakter double-width"""
    length = 0
    for char in text:
        if unicodedata.east_asian_width(char) in ('W', 'F'):
            length += 2
        else:
            length += 1
    return length

def render(lyric: str, snow: list[list[str]]) -> str:
    """Mengatur pergerakan salju estetik dan menampilkan lirik dengan benar"""
    
    # Geser salju ke bawah (logic dari bawah ke atas)
    for r in range(HEIGHT - 1, 0, -1):
        for c in range(WIDTH):
            if snow[r - 1][c] != " ":
                # Kecepatan sedang
                if random.random() < 0.09:
                    snow[r][c] = snow[r - 1][c]
                    snow[r - 1][c] = " "

    # Bersihkan baris paling bawah secara berkala
    for c in range(WIDTH):
        if random.random() < 0.10:
            snow[HEIGHT - 1][c] = " "

    # Spawn salju baru di baris paling atas
    for c in range(WIDTH):
        if snow[0][c] == " ":
            # PERBAIKAN: Peluang diturunkan ke 0.015 agar salju lebih sedikit & bersih
            snow[0][c] = random.choice(FLAKES) if random.random() < 0.010 else " "
    
    # Duplikat frame untuk rendering teks tanpa merusak array utama salju
    frame = [row[:] for row in snow]
    mid_row = HEIGHT * 3 // 5
    
    # Menghitung posisi tengah lirik berdasarkan panjang visual
    lyric_visual_len = get_visual_length(lyric)
    start_col = (WIDTH - lyric_visual_len) // 2
    
    # Beri ruang kosong di sekitar lirik
    for r_offset in [-1, 0, 1]:
        if 0 <= mid_row + r_offset < HEIGHT:
            frame[mid_row + r_offset] = [" "] * WIDTH
            
    # Tempel lirik di tengah layar
    col = start_col
    for ch in lyric:
        char_len = 2 if unicodedata.east_asian_width(ch) in ('W', 'F') else 1
        if 0 <= col < WIDTH and col + char_len <= WIDTH:
            frame[mid_row][col] = ch
            if char_len == 2:
                frame[mid_row][col+1] = "" 
            col += char_len
        else:
            col += char_len
            
    return "\n".join("".join(row) for row in frame)

def render_simple_text(lines, pad_rows=0):
    """Fungsi pembantu untuk menampilkan teks diam (judul/closing) di tengah layar bersih"""
    total_text_rows = len(lines)
    start_row = (HEIGHT - total_text_rows) // 2 + pad_rows
    
    # Membuat canvas kosong
    frame = [[" "] * WIDTH for _ in range(HEIGHT)]
    
    # Menempel teks baris per baris
    for i, text in enumerate(lines):
        r = start_row + i
        if 0 <= r < HEIGHT:
            visual_len = get_visual_length(text)
            c = (WIDTH - visual_len) // 2
            
            curr_c = c
            for ch in text:
                char_len = 2 if unicodedata.east_asian_width(ch) in ('W', 'F') else 1
                if 0 <= curr_c < WIDTH and curr_c + char_len <= WIDTH:
                    frame[r][curr_c] = ch
                    if char_len == 2:
                        frame[r][curr_c+1] = ""
                    curr_c += char_len
                else:
                    curr_c += char_len

    return "\n".join("".join(row) for row in frame)

def main():
    # Diinisialisasi kosong total agar salju turun bertahap
    snow = [[" "] * WIDTH for _ in range(HEIGHT)]
    pad = get_padding()
    
    try:
        hide_cursor()
        clear()
        

        title_lines = [
            "⋆꙳•❅*‧ ‧*❆ ₊⋆",
            "I Miss You",
            "Ghost",
            "⋆꙳•❅*‧ ‧*❆ ₊⋆"
        ]
        
        title_output = render_simple_text(title_lines, pad_rows=-1)
        sys.stdout.write("\033[H" + "\n" * pad + title_output)
        sys.stdout.flush()
        time.sleep(3.0)
        

        deadline = time.time() + 4.0
        while time.time() < deadline:
            sys.stdout.write("\033[H" + "\n" * pad + render("", snow))
            sys.stdout.flush()
            time.sleep(FPS)
            

        for lyric, duration in LYRICS:
            deadline = time.time() + duration
            while time.time() < deadline:
                sys.stdout.write("\033[H" + "\n" * pad + render(lyric, snow))
                sys.stdout.flush()
                time.sleep(FPS)
                

        clear()
        closing_lines = ["i loved u"]
        closing_output = render_simple_text(closing_lines)
        sys.stdout.write("\033[H" + "\n" * pad + closing_output)
        sys.stdout.flush()
        time.sleep(3.5)
        clear()

    except KeyboardInterrupt:
        clear()
    finally:
        show_cursor()

if __name__ == "__main__":
    main()