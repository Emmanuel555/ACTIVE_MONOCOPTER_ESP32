ESP32 Udev Rules:

curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
sudo usermod -aG dialout $USER

Then log out, log back in, unplug/replug the ESP32, and try upload again.

use ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null to test if ESP32 is connected

it shud print /dev/ttyACM0


Extra:

**PlatformIO only works with VScode

rmb to run sudo apt install python3-venv

