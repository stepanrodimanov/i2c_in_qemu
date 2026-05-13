#include "qemu/osdep.h"
#include "hw/i2c/i2c.h"
#include "hw/core/qdev-properties.h"
#include "migration/vmstate.h"
#include "qemu/module.h"
#include "qom/object.h"

#define TYPE_MY_I2C_DEVICE "my-i2c-device"
OBJECT_DECLARE_SIMPLE_TYPE(MyI2CDeviceState, MY_I2C_DEVICE)

struct MyI2CDeviceState {
    I2CSlave parent_obj;
    uint8_t reg;
};

static int my_i2c_event(I2CSlave *s, enum i2c_event event)
{
    return 0;
}

static uint8_t my_i2c_recv(I2CSlave *s)
{
    MyI2CDeviceState *dev = MY_I2C_DEVICE(s);
    return dev->reg;
}

static int my_i2c_send(I2CSlave *s, uint8_t data)
{
    MyI2CDeviceState *dev = MY_I2C_DEVICE(s);
    dev->reg = data;
    return 0;
}

static const VMStateDescription vmstate_my_i2c_device = {
    .name = TYPE_MY_I2C_DEVICE,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (const VMStateField[]) {
        VMSTATE_I2C_SLAVE(parent_obj, MyI2CDeviceState),
        VMSTATE_UINT8(reg, MyI2CDeviceState),
        VMSTATE_END_OF_LIST()
    },
};

static const Property my_i2c_device_properties[] = {
    DEFINE_PROP_UINT8("reg", MyI2CDeviceState, reg, 0x00),
};

static void my_i2c_device_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *sc = I2C_SLAVE_CLASS(klass);

    dc->vmsd = &vmstate_my_i2c_device;
    device_class_set_props(dc, my_i2c_device_properties);

    sc->event = my_i2c_event;
    sc->recv = my_i2c_recv;
    sc->send = my_i2c_send;
}

static const TypeInfo my_i2c_device_info = {
    .name = TYPE_MY_I2C_DEVICE,
    .parent = TYPE_I2C_SLAVE,
    .instance_size = sizeof(MyI2CDeviceState),
    .class_init = my_i2c_device_class_init,
};

static void my_i2c_device_register_types(void)
{
    type_register_static(&my_i2c_device_info);
}

type_init(my_i2c_device_register_types)
