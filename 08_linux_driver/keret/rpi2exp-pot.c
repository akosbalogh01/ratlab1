/*
 * Copyright (C) 2015 
 *	Gabor Banyasz <banyasz.gabor@aut.bme.hu>
 * 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/i2c.h>

#if defined(CONFIG_OF)
#include <linux/of.h>
#endif

enum chips {
	mcp40d18,
};

/* Az attribútum lekérdező függvénye */
static ssize_t show_wiper(struct device *dev, struct device_attribute *attr,
		       char *buf)
{
  struct i2c_client *client = to_i2c_client(dev);
  int ret;
  
  // TODO: érték beolvasása és hibaellenőrzés
  ret = i2c_smbus_read_byte_data(client, 0);
  if (ret < 0) {
    return ret;
  }

  return sprintf(buf, "%d\n", ret);
}


/* Az attribútum beállító függvénye */
static ssize_t set_wiper(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
  struct i2c_client *client = to_i2c_client(dev);
  int ret;
  u8 value;

  if(sscanf(buf, "%hhu", &value) == 1) {
    // TODO: tartomány ellenőrzés
    if (value > 127) {
      return -EINVAL;
    }

    // TODO: érték elküldése és hibaellenőrzés
    ret = i2c_smbus_write_byte_data(client, 0, value);
    if (ret < 0) {
      return ret;
    }
    return count;
  }
  
  return -EINVAL;
}


/* Az attribútum összeállítása */
static DEVICE_ATTR(wiper, S_IRUGO | S_IWUSR, show_wiper, set_wiper);

/* Az eszköz probe függvénye */
static int rpi2exp_pot_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  int ret;
  
  dev_info(&client->dev, "probe");
  
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA)) {
    dev_err(&client->dev, "functionality check failed");
    return -ENODEV;
  }
  
  // TODO: eszköz kommunikáció ellenőrzése
  ret = i2c_smbus_read_byte_data(client, 0);
  if (ret < 0) {
    return ret;
  }

  // TODO: attribútum állomány létrehozása
  device_create_file(&client->dev, &dev_attr_wiper);
  
  return 0;
}


/* Az eszköz felszabadító függvénye */
static int rpi2exp_pot_remove(struct i2c_client *client)
{
  dev_info(&client->dev, "remove");
  
  // TODO: attribútum állomány megszüntetése
  device_remove_file(&client->dev, &dev_attr_wiper);
  
  return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id rpi2exp_pot_dt[] = {
  { .compatible = "rpi2exp-pot" },
  { },
};
MODULE_DEVICE_TABLE(of, rpi2exp_pot_dt);
#endif

static const struct i2c_device_id rpi2exp_pot_id[] = {
	{ "mcp40d18", mcp40d18 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rpi2exp_pot_id);

static struct i2c_driver rpi2exp_pot_driver = {
  .driver	= {
    .name	= "rpi2exp-pot",
    .owner	= THIS_MODULE,
#if defined(CONFIG_OF)
    .of_match_table = of_match_ptr(rpi2exp_pot_dt),
#endif
  },
  .probe	= rpi2exp_pot_probe,
  .remove	= rpi2exp_pot_remove,
  .id_table = rpi2exp_pot_id,
};

module_i2c_driver(rpi2exp_pot_driver);

MODULE_DESCRIPTION("RPI2EXP Digital POT (MCP40D18) driver");
MODULE_LICENSE("GPL");
