# Arduino_Project

Repo ini berisi beberapa project arduino yang pernah dibuat selama kuliah.

## tips fix port tidak terdetek di linux
### 1. Remove undev rules
   for f in /usr/lib/udev/rules.d/*brltty*.rules; do
   sudo ln -s /dev/null "/etc/udev/rules.d/$(basename "$f")"
   done
   sudo udevadm control --reload-rules
   
### 2. Disable Service
   $ sudo systemctl mask brltty.path
   Output => Created symlink /etc/systemd/system/brltty.path → /dev/null.
   
### 3. Add yourself to the dialout group
   sudo usermod -a -G dialout $USER

   Referensi link = https://unix.stackexchange.com/questions/670636/unable-to-use-usb-dongle-based-on-usb-serial-converter
   chip/670637#670637

