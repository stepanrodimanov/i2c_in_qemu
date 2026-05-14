# I2C in QEMU

## Preparing a Guest Persona

```
mkdir images
cd images
wget https://cloud.debian.org/images/cloud/bookworm/latest/debian-12-nocloud-amd64.qcow2
cd ..
```


## Build QEMU with the I2C device

```
git clone https://gitlab.com/qemu-project/qemu
cd qemu

cp ../src/my_i2c_device.c hw/i2c/
sed -i '/system_ss.add_all/i i2c_ss.add(when: '\''CONFIG_MY_I2C_DEVICE'\'', if_true: files('\''my_i2c_device.c'\''))' hw/i2c/meson.build
cat <<EOF >> hw/i2c/Kconfig

config MY_I2C_DEVICE
    bool
    default y
    select I2C
EOF

mkdir build && cd build
../configure --target-list=x86_64-softmmu --enable-debug
make -j$(nproc)
```


## Run

```
./qemu-system-x86_64 \
    -m 1024 \
    -hda ../../images/debian-12-nocloud-amd64.qcow2 \
    -device my-i2c-device,id=i2c,address=0x60 \
    -enable-kvm \
    -nographic \
    -qmp unix:/tmp/qmp.sock,server=on,wait=off
```
Login: root (no password)

In another terminal
```
python qmp.py
```

Inside the VM:
```
apt update && apt install -y i2c-tools
modprobe i2c-dev
i2cdetect -y 0 
i2cget -y 0 0x60 
i2cset -y 0 0x60 0x11 
```
