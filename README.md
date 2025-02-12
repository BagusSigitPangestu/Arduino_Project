# Arduino Project

Repository ini berisi beberapa proyek Arduino yang telah dibuat selama kuliah.

## 🔧 Tips Mengatasi Port Arduino Tidak Terdeteksi di Linux

Jika port serial tidak terdeteksi saat menggunakan Arduino di Linux, coba jalankan perintah berikut secara berurutan:

### 1️⃣ Hapus Udev Rules yang Bermasalah
Jalankan perintah berikut untuk menghapus aturan `udev` yang dapat menyebabkan konflik:
```bash
for f in /usr/lib/udev/rules.d/*brltty*.rules; do
    sudo ln -s /dev/null "/etc/udev/rules.d/$(basename "$f")"
done
sudo udevadm control --reload-rules
```

### 2️⃣ Nonaktifkan Layanan `brltty`
Nonaktifkan `brltty` (Braille Terminal Daemon) yang dapat mengganggu koneksi serial:
```bash
sudo systemctl mask brltty.path
```

### 3️⃣ Tambahkan Pengguna ke Grup `dialout`
Pastikan akun pengguna memiliki akses ke perangkat serial dengan menambahkan ke grup `dialout`:
```bash
sudo usermod -a -G dialout $USER
```
Setelah itu, logout dan login kembali agar perubahan diterapkan.

---

📌 **Referensi:**  
[Unable to use USB dongle based on USB-serial converter](https://unix.stackexchange.com/questions/670636/unable-to-use-usb-dongle-based-on-usb-serial-converter-chip/670637#670637)
