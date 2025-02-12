# Arduino Project

Repository ini berisi beberapa proyek Arduino yang telah dibuat selama kuliah.

## 🔧 Tips Mengatasi Port Arduino Tidak Terdeteksi di Linux

Jika port serial tidak terdeteksi saat menggunakan Arduino di Linux, coba jalankan perintah berikut secara berurutan:

```bash
# 1️⃣ Hapus Udev Rules yang Bermasalah
for f in /usr/lib/udev/rules.d/*brltty*.rules; do
    sudo ln -s /dev/null "/etc/udev/rules.d/$(basename "$f")"
done
sudo udevadm control --reload-rules

# 2️⃣ Nonaktifkan Layanan brltty
sudo systemctl mask brltty.path
# Jika berhasil, akan muncul output:
# Created symlink /etc/systemd/system/brltty.path → /dev/null.

# 3️⃣ Tambahkan Pengguna ke Grup dialout
sudo usermod -a -G dialout $USER
# Setelah itu, logout dan login kembali agar perubahan diterapkan.

# 🔗 Referensi:
# https://unix.stackexchange.com/questions/670636/unable-to-use-usb-dongle-based-on-usb-serial-converter-chip/670637#670637
