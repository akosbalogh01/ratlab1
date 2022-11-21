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
	mcp3021,
};

/* Eszköz kommunikáció */
static int rpi2exp_adc_read(struct device *dev)
{
  struct i2c_client *client = to_i2c_client(dev);
  int ret;
  u16 value;
  u16 recvbuf;

  // TODO: érték beolvasása és hibaellenőrzés
  ret = i2c_master_recv(client, (char*) &recvbuf, 2);
  if (ret < 0) {
    return ret;
  }

  // TODO: érték bájtsorrendjének átalakítása ( be16_to_cpu() )
  value = be16_to_cpu(recvbuf);

  // TODO: a nem használt bitek eltávolítása
  value = ((value & 0x0FFF) >> 2);
  return value;
}


/* Számérték -> feszültség konverzió */
static u16 mV_from_value(u16 value)
{
  if(value == 0)
    return 0;
  
  return DIV_ROUND_CLOSEST(value * 5000, (1 << 10));
}

/* Az attribútum lekérdező függvénye */
static ssize_t show_raw(struct device *dev, struct device_attribute *attr,
		       char *buf)
{
  int ret;
  
  ret = rpi2exp_adc_read(dev);
  if(ret < 0)
    return ret;
  
  return sprintf(buf, "%d\n", ret);
}

/* Az attribútum lekérdező függvénye */
static ssize_t show_in(struct device *dev, struct device_attribute *attr,
		       char *buf)
{
  int ret;
  
  ret = rpi2exp_adc_read(dev);
  if(ret < 0)
    return ret;
  
  return sprintf(buf, "%d\n", mV_from_value(ret));
}

/* Az attribútum összeállítása */
static DEVICE_ATTR(raw, S_IRUGO, show_raw, NULL);
static DEVICE_ATTR(in, S_IRUGO, show_in, NULL);

/* Az eszköz probe függvénye */
static int rpi2exp_adc_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  int ret;
  dev_info(&client->dev, "probe");
  
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
    dev_err(&client->dev, "functionality check failed");
    return -ENODEV;
  }

  // TODO: eszköz kommunikáció ellenőrzése
  ret = i2c_master_send(client, NULL, 0);
  if (ret < 0) {
    return ret;
  }

  // TODO: attribútum állományok létrehozása
  device_create_file(&client->dev, &dev_attr_raw);
  device_create_file(&client->dev, &dev_attr_in);
  
  return 0;
}


/* Az eszköz felszabadító függvénye */
static int rpi2exp_adc_remove(struct i2c_client *client)
{
  dev_info(&client->dev, "remove");
  
  // TODO: attribútum állományok megszüntetése
  device_remove_file(&client->dev, &dev_attr_raw);
  device_remove_file(&client->dev, &dev_attr_in);
  
  return 0;
}


#if defined(CONFIG_OF)
static const struct of_device_id rpi2exp_adc_dt[] = {
  { .compatible = "rpi2exp-adc" },
  { },
};
MODULE_DEVICE_TABLE(of, rpi2exp_adc_dt);
#endif

static const struct i2c_device_id rpi2exp_adc_id[] = {
	{ "mcp3021", mcp3021 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rpi2exp_adc_id);

static struct i2c_driver rpi2exp_adc_driver = {
  .driver	= {
    .name	= "rpi2exp-adc",
    .owner	= THIS_MODULE,
#if defined(CONFIG_OF)
    .of_match_table = of_match_ptr(rpi2exp_adc_dt),
#endif
  },
  .probe	= rpi2exp_adc_probe,
  .remove	= rpi2exp_adc_remove,
  .id_table = rpi2exp_adc_id,
};

module_i2c_driver(rpi2exp_adc_driver);

MODULE_DESCRIPTION("RPI2EXP ADC (MCP3021) driver");
MODULE_LICENSE("GPL");
