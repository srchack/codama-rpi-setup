#!/usr/bin/env bash
pushd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null
RPI_SETUP_DIR="$( pwd )"

# Disable the built-in audio output so there is only one audio
# device in the system
sudo sed -i -e 's/dtparam=audio=on/#dtparam=audio=on/' /boot/config.txt

# Enable the i2s device tree
sudo sed -i -e 's/#dtparam=i2s=on/dtparam=i2s=on/' /boot/config.txt

echo "Installing Raspberry Pi kernel headers"
sudo apt-get install -y raspberrypi-kernel-headers


# Build driver and insert it into the kernel
if [ "`uname -m`" = "armv6l" ] ; then
    sed -i 's/3f203000\.i2s/20203000\.i2s/' codama-kmod/snd-soc-codama-soundcard.c
else
    sed -i 's/20203000\.i2s/3f203000\.i2s/' codama-kmod/snd-soc-codama-soundcard.c
fi
pushd $RPI_SETUP_DIR/codama-kmod > /dev/null
make all
sudo make install
popd > /dev/null


# Move existing files to back up
if [ -e ~/.asoundrc ] ; then
    chmod a+w ~/.asoundrc
    cp ~/.asoundrc ~/.asoundrc.bak
fi
if [ -e /usr/share/alsa/pulse-alsa.conf ] ; then
    sudo mv /usr/share/alsa/pulse-alsa.conf  /usr/share/alsa/pulse-alsa.conf.bak
    sudo mv ~/.config/lxpanel/LXDE-pi/panels/panel ~/.config/lxpanel/LXDE-pi/panels/panel.bak
fi

# Check args for asoundrc selection. Default to VF Stereo.
cp $RPI_SETUP_DIR/resources/asoundrc_vf_codama ~/.asoundrc

cp $RPI_SETUP_DIR/resources/panel ~/.config/lxpanel/LXDE-pi/panels/panel

# Make the asoundrc file read-only otherwise lxpanel rewrites it
# as it doesn't support anything but a hardware type device
chmod a-w ~/.asoundrc


# Apply changes
sudo /etc/init.d/alsa-utils restart


# Configure the I2C - disable the default built-in driver
sudo sed -i -e 's/#\?dtparam=i2c_arm=on/dtparam=i2c_arm=off/' /boot/config.txt
if ! grep -q "i2c-bcm2708" /etc/modules-load.d/modules.conf; then
  sudo sh -c 'echo i2c-bcm2708 >> /etc/modules-load.d/modules.conf'
fi
if ! grep -q "i2c-dev" /etc/modules-load.d/modules.conf; then
  sudo sh -c 'echo i2c-dev >> /etc/modules-load.d/modules.conf'
fi
if ! grep -q "options i2c-bcm2708 combined=1" /etc/modprobe.d/i2c.conf; then
  sudo sh -c 'echo "options i2c-bcm2708 combined=1" >> /etc/modprobe.d/i2c.conf'
fi


# Build a new I2C driver
pushd $RPI_SETUP_DIR/i2c-gpio-param > /dev/null
make || exit $?
popd > /dev/null


# setting to load module into the kernel
sudo cp $RPI_SETUP_DIR/i2c-gpio-param/i2c-gpio-param.ko /lib/modules/`uname -r`/kernel/drivers/i2c/
sudo depmod -ae
if ! grep -q "i2c-gpio-param" /etc/modules-load.d/modules.conf; then
    sudo sed -i -e '$ a i2c-gpio-param' /etc/modules-load.d/modules.conf
fi
if ! grep -q "options i2c-gpio-param busid=1 sda=2 scl=3 udelay=5 timeout=100 sda_od=0 scl_od=0 scl_oo=0" /etc/modprobe.d/i2c.conf; then
    sudo sed -i -e '$ a options i2c-gpio-param busid=1 sda=2 scl=3 udelay=5 timeout=100 sda_od=0 scl_od=0 scl_oo=0' /etc/modprobe.d/i2c.conf
fi


# insert startup so that alsamixer configures
sed -e '$i \# Run Alsa at startup so that alsamixer configures\n' rc.local
sed -e '$i \arecord -d 1 > /dev/null 2>&1\n' rc.local
sed -e '$i \aplay dummy > /dev/null 2>&1\n' rc.local


echo "To enable I2S, this Raspberry Pi must be rebooted."

popd > /dev/null
