/*
 * Copyright (C) 2015 
 *	Gabor Banyasz <banyasz.gabor@aut.bme.hu>
 * 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>

#if defined(CONFIG_OF)
#include <linux/of.h>
#include <linux/of_gpio.h>
#endif

/* GPIO beállítások tárolása */
struct rpi2exp_lcd {
  unsigned rs;
  unsigned bl;
};

/* LCD inicializálás */
static int rpi2exp_lcd_init(struct device* dev)
{
  struct rpi2exp_lcd *mydev = dev_get_drvdata(dev);
  struct spi_device *spi = to_spi_device(dev);
  
  // EA DOGM162 8 bit / 5V
  static unsigned char tx[] = { 
    0x39,
    0x1c,
    0x52,
    0x69,
    0x74,
    0x38,
    0x0f,
    0x01,
    0x06,
  };
  
  // TODO: RS vonal beállítása
  spi_write(spi, tx, sizeof(tx));

  return 0;
}

/* Az attribútum beállító függvénye */
static ssize_t set_line(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
  // TODO: a sor index kitalálása attribútum név alapján
  
  // TODO: pozíció (DDRAM cím) beállítása
  // Első sor: 0x00, második sor: 0x40
  // A törlést hanyagoljuk, mert lassú.
  
  // TODO: a sor tartalmának elküldése
  
  return count;
}

/* Az attribútum összeállítása */
static DEVICE_ATTR(line0, S_IWUSR, NULL, set_line);
static DEVICE_ATTR(line1, S_IWUSR, NULL, set_line);

/* Az eszköz probe függvénye */
static int rpi2exp_lcd_probe(struct spi_device *spi)
{
  struct rpi2exp_lcd *mydev;
  #if defined(CONFIG_OF)
  struct device_node* np = spi->dev.of_node;
  #endif
  
  dev_info(&spi->dev, "probe");
  
  mydev = devm_kzalloc(&spi->dev,
		       sizeof(struct rpi2exp_lcd),
		       GFP_KERNEL);
  
  if (!mydev) {
    dev_err(&spi->dev, "out of memory!\n");
    return -ENOMEM;
  }
  
  #if defined(CONFIG_OF)
  mydev->rs = of_get_named_gpio(np, "rs-gpio", 0);
  mydev->bl = of_get_named_gpio(np, "bl-gpio", 0);
  #endif
  dev_set_drvdata(&spi->dev, mydev);
  
  // TODO: GPIO-k ellenőrzése, lefoglalása, inicializálása
  
  rpi2exp_lcd_init(&spi->dev);
  
  // TODO: háttérvilágítás bekapcsolása
  
  // TODO: attribútum állományok létrehozása
  
  return 0;
}

/* Az eszköz felszabadító függvénye */
static int rpi2exp_lcd_remove(struct spi_device *spi)
{
  dev_info(&spi->dev, "remove");

  // TODO: attribútum állományok megszüntetése
  
  return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id rpi2exp_lcd_dt[] = {
  { .compatible = "rpi2exp-lcd" },
  { },
};
MODULE_DEVICE_TABLE(of, rpi2exp_lcd_dt);
#endif

static struct spi_driver rpi2exp_lcd_driver = {
  .driver	= {
    .name	= "rpi2exp-lcd",
    .owner	= THIS_MODULE,
    #if defined(CONFIG_OF)
    .of_match_table = of_match_ptr(rpi2exp_lcd_dt),
    #endif
  },
  .probe	= rpi2exp_lcd_probe,
  .remove	= rpi2exp_lcd_remove,
};

module_spi_driver(rpi2exp_lcd_driver);

MODULE_DESCRIPTION("RPI2EXP LCD (EA DOGM162) driver");
MODULE_LICENSE("GPL");
