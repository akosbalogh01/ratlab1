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
	tcn75a,
};

/* Az attribútum lekérdező függvénye */
static ssize_t show_temp(struct device *dev, struct device_attribute *attr,
		       char *buf)
{
  struct i2c_client *client = to_i2c_client(dev);
  int ret;
  u16 value;
  u8 sign, round, fract;
  
  // TODO: érték beolvasása és hibaellenőrzés
  ret = i2c_smbus_read_word_data(client, 0);
  if (ret < 0) {
    return ret;
  }

  // TODO: érték bájtsorrendjének átalakítása ( be16_to_cpu() )
  value = be16_to_cpu(ret);

  // TODO: az érték szöveges kiírása (alapértelmezett fél fokos felbontás)
  sign = (value >> 15);
  round = ((value & 0x7FFF) >> 8);
  fract = ((value & 0x00FF) >> 7);

  if (sign && fract)
    return sprintf(buf, "-%d.5\n", round);
  else if (sign && !fract)
    return sprintf(buf, "-%d.0\n", round);
  else if (!sign && fract)
    return sprintf(buf, "%d.5\n", round);
  else
    return sprintf(buf, "%d.0\n", round);
}


/* Az attribútum összeállítása */
static DEVICE_ATTR(temp, S_IRUGO, show_temp, NULL);

/* Az eszköz probe függvénye */
static int rpi2exp_temp_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  int ret;
  
  dev_info(&client->dev, "probe");
  
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA)) {
    dev_err(&client->dev, "functionality check failed");
    return -ENODEV;
  }
  
  // TODO: eszköz kommunikáció ellenőrzése
  ret = i2c_smbus_read_word_data(client, 0);
  if (ret < 0) {
    return ret;
  }

  // TODO: attribútum állományok létrehozása
  device_create_file(&client->dev, &dev_attr_temp);

  return 0;
}


/* Az eszköz felszabadító függvénye */
static int rpi2exp_temp_remove(struct i2c_client *client)
{
  dev_info(&client->dev, "remove");
  
  // TODO: attribútum állományok megszüntetése
  device_remove_file(&client->dev, &dev_attr_temp);

  return 0;
}


#if defined(CONFIG_OF)
static const struct of_device_id rpi2exp_temp_dt[] = {
  { .compatible = "rpi2exp-temp" },
  { },
};
MODULE_DEVICE_TABLE(of, rpi2exp_temp_dt);
#endif

static const struct i2c_device_id rpi2exp_temp_id[] = {
	{ "tcn75a", tcn75a },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rpi2exp_temp_id);

static struct i2c_driver rpi2exp_temp_driver = {
  .driver	= {
    .name	= "rpi2exp-temp",
    .owner	= THIS_MODULE,
#if defined(CONFIG_OF)
    .of_match_table = of_match_ptr(rpi2exp_temp_dt),
#endif
  },
  .probe	= rpi2exp_temp_probe,
  .remove	= rpi2exp_temp_remove,
  .id_table = rpi2exp_temp_id,
};

module_i2c_driver(rpi2exp_temp_driver);

MODULE_DESCRIPTION("RPI2EXP Temperature (TCN75A) driver");
MODULE_LICENSE("GPL");
