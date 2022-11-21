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
#include <linux/platform_device.h>

#if defined(CONFIG_OF)
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#endif

/* LED állapotokat leíró változók */
struct rpi2exp_leds {
  unsigned led_gpios[4];
};

/* GPIO-k lefoglalása és inicializálása */
static int rpi2exp_leds_init_led(struct device* dev, unsigned ix)
{
  struct rpi2exp_leds *mydev = dev_get_drvdata(dev);
  char label[8];
  
  if(ix >= 4) return -EINVAL;
  /* GPIO érték ellenőrzése */
  if(!gpio_is_valid(mydev->led_gpios[ix])) return -EINVAL;
  
  sprintf(label, "led%u", ix);
  /* GPIO lefoglalása az eszközhöz */
  if(devm_gpio_request(dev, mydev->led_gpios[ix], label))  {
    dev_err(dev, "%s(%u) request error!", label, mydev->led_gpios[ix]);
    return -EINVAL;
  }
  
  /* GPIO kimeneti módba kapcsolása */
  gpio_direction_output(mydev->led_gpios[ix], 1);
  
  return 0;
}  

/* LED attribútum beállító függvénye */
static ssize_t set_led(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
  struct rpi2exp_leds *mydev = dev_get_drvdata(dev);
  int i, val;
  
  /* Az index visszafejtése az attribútum neve alapján. */
  if((sscanf(attr->attr.name, "led%d", &i) == 1) && (i >= 1) && (i <= 4)) {
    if(sscanf(buf, "%d", &val) == 1) {
      /* A GPIO értékének beállítása */
      gpio_set_value(mydev->led_gpios[i - 1], (val == 0) ? 1 : 0);
      return count;
    }
  }
  return -EINVAL;
}

/* LED attribútum állományok paramétereinek beállítása */
DEVICE_ATTR(led1, S_IWUSR, NULL, set_led);
DEVICE_ATTR(led2, S_IWUSR, NULL, set_led);
DEVICE_ATTR(led3, S_IWUSR, NULL, set_led);
DEVICE_ATTR(led4, S_IWUSR, NULL, set_led);

/* Az eszköz probe függvénye */
static int rpi2exp_leds_probe(struct platform_device *pdev)
{
  struct rpi2exp_leds *mydev;
  int i, ret;
#if defined(CONFIG_OF)
  struct device_node* np = pdev->dev.of_node;
#endif
  
  dev_info(&pdev->dev, "probe");
  
  /* Eszközhöz kapcsolódó memória allokáció */
  mydev = devm_kzalloc(&pdev->dev,
		       sizeof(struct rpi2exp_leds),
		       GFP_KERNEL);
  
  if (!mydev) {
    dev_err(&pdev->dev, "out of memory!\n");
    return -ENOMEM;
  }
  
#if defined(CONFIG_OF)
  /* Device tree beállítások beolvasása */
  mydev->led_gpios[0] = of_get_named_gpio(np, "led1-gpio", 0);
  mydev->led_gpios[1] = of_get_named_gpio(np, "led2-gpio", 0);
  mydev->led_gpios[2] = of_get_named_gpio(np, "led3-gpio", 0);
  mydev->led_gpios[3] = of_get_named_gpio(np, "led4-gpio", 0);
#endif
  /* Saját leíró struktúra hozzárendelése az eszköz leíró struktúrájához */
  platform_set_drvdata(pdev, mydev);
  
  /* LED GPIO-k foglalása és inicializálása */
  for(i = 0; i < 4; i++) {
    ret = rpi2exp_leds_init_led(&pdev->dev, i);
    if(ret < 0)
      return ret;
  }

  /* Attribútum állományok létrehozása */
  device_create_file(&pdev->dev, &dev_attr_led1);
  device_create_file(&pdev->dev, &dev_attr_led2);
  device_create_file(&pdev->dev, &dev_attr_led3);
  device_create_file(&pdev->dev, &dev_attr_led4);

  return 0;
}

/* Az eszköz felszabadító függvénye */
static int rpi2exp_leds_remove(struct platform_device *pdev)
{
  dev_info(&pdev->dev, "remove");
  
  /* Attribútum állományok megszüntetése */
  device_remove_file(&pdev->dev, &dev_attr_led1);
  device_remove_file(&pdev->dev, &dev_attr_led2);
  device_remove_file(&pdev->dev, &dev_attr_led3);
  device_remove_file(&pdev->dev, &dev_attr_led4);
  
  return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id rpi2exp_leds_dt[] = {
  { .compatible = "rpi2exp-leds" },
  { },
};
MODULE_DEVICE_TABLE(of, rpi2exp_leds_dt);
#endif

static struct platform_driver rpi2exp_leds_driver = {
  .driver	= {
    .name	= "rpi2exp-leds",
    .owner	= THIS_MODULE,
#if defined(CONFIG_OF)
    .of_match_table = of_match_ptr(rpi2exp_leds_dt),
#endif
  },
  .probe	= rpi2exp_leds_probe,
  .remove	= rpi2exp_leds_remove,
};

module_platform_driver(rpi2exp_leds_driver);

MODULE_DESCRIPTION("RPI2EXP GPIO LED driver");
MODULE_AUTHOR("Gabor Banyasz <banyasz.gabor@aut.bme.hu>");
MODULE_LICENSE("GPL");
