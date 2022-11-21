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

/* Nyomógomb állapotokat leíró változók */
struct rpi2exp_btns {
  unsigned btn_gpios[4];
};

/* GPIO-k lefoglalása és inicializálása */
static int rpi2exp_btns_init_btn(struct device* dev, unsigned ix)
{
  // TODO: lásd LED inicializálása, viszont input mód!
  struct rpi2exp_btns *mydev = dev_get_drvdata(dev);
  char label[8];

  if(ix >= 4) return -EINVAL;
  /* GPIO érték ellenőrzése */
  if(!gpio_is_valid(mydev->btn_gpios[ix])) return -EINVAL;

  sprintf(label, "btn%u", ix);
  /* GPIO lefoglalása az eszközhöz */
  if(devm_gpio_request(dev, mydev->btn_gpios[ix], label))  {
    dev_err(dev, "%s(%u) request error!", label, mydev->btn_gpios[ix]);
    return -EINVAL;
  }

  /* GPIO bemeneti módba kapcsolása */
  gpio_direction_input(mydev->btn_gpios[ix]);

  return 0;
}  

/* Nyomógomb attribútum lekérdező függvénye */
static ssize_t show_btn(struct device* dev, struct device_attribute* attr, char* buf)
{
  // TODO: GPIO érték visszaadása
  struct rpi2exp_btns *mydev = dev_get_drvdata(dev);
  int i;
  ssize_t count;

  /* Az index visszafejtése az attribútum neve alapján. */
  if((sscanf(attr->attr.name, "btn%d", &i) == 1) && (i >= 1) && (i <= 4)) {
    count = sprintf(buf, "%d", gpio_get_value(mydev->btn_gpios[i - 1]));
    if(count > 0) {
      return count;
    }
  }

  return -EINVAL;
}


/* Az attribútum összeállítása */
DEVICE_ATTR(btn1, S_IRUSR, show_btn, NULL);
DEVICE_ATTR(btn2, S_IRUSR, show_btn, NULL);
DEVICE_ATTR(btn3, S_IRUSR, show_btn, NULL);
DEVICE_ATTR(btn4, S_IRUSR, show_btn, NULL);

/* Az eszköz probe függvénye */
static int rpi2exp_btns_probe(struct platform_device *pdev)
{
  struct rpi2exp_btns *mydev;
  int i, ret;
#if defined(CONFIG_OF)
  struct device_node* np = pdev->dev.of_node;
#endif
  
  dev_info(&pdev->dev, "probe");
  
  mydev = devm_kzalloc(&pdev->dev,
		       sizeof(struct rpi2exp_btns),
		       GFP_KERNEL);
  
  if (!mydev) {
    dev_err(&pdev->dev, "out of memory!\n");
    return -ENOMEM;
  }
  
#if defined(CONFIG_OF)
  mydev->btn_gpios[0] = of_get_named_gpio(np, "btn1-gpio", 0);
  mydev->btn_gpios[1] = of_get_named_gpio(np, "btn2-gpio", 0);
  mydev->btn_gpios[2] = of_get_named_gpio(np, "btn3-gpio", 0);
  mydev->btn_gpios[3] = of_get_named_gpio(np, "btn4-gpio", 0);
#endif
  platform_set_drvdata(pdev, mydev);
  
  // TODO: GPIO-k inicializálása
  for(i = 0; i < 4; i++) {
    ret = rpi2exp_btns_init_btn(&pdev->dev, i);
    if(ret < 0)
      return ret;
  }

  // TODO: attribútum állományok létrehozása
  device_create_file(&pdev->dev, &dev_attr_btn1);
  device_create_file(&pdev->dev, &dev_attr_btn2);
  device_create_file(&pdev->dev, &dev_attr_btn3);
  device_create_file(&pdev->dev, &dev_attr_btn4);

  return 0;
}

/* Az eszköz felszabadító függvénye */
static int rpi2exp_btns_remove(struct platform_device *pdev)
{
  dev_info(&pdev->dev, "remove");
  
  // TODO: attribútum állományok megszüntetése
  
  return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id rpi2exp_btns_dt[] = {
  { .compatible = "rpi2exp-btns" },
  { },
};
MODULE_DEVICE_TABLE(of, rpi2exp_btns_dt);
#endif

static struct platform_driver rpi2exp_btns_driver = {
  .driver	= {
    .name	= "rpi2exp-btns",
    .owner	= THIS_MODULE,
#if defined(CONFIG_OF)
    .of_match_table = of_match_ptr(rpi2exp_btns_dt),
#endif
  },
  .probe	= rpi2exp_btns_probe,
  .remove	= rpi2exp_btns_remove,
};

module_platform_driver(rpi2exp_btns_driver);

MODULE_DESCRIPTION("RPI2EXP GPIO Button driver");
MODULE_LICENSE("GPL");
