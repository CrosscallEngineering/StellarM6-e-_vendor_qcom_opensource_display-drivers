// SPDX-License-Identifier: GPL-2.0-only
/*
 * LEDs driver for Sunlight
 *
 * Copyright (C) 2020 HMD
 *
 * Based on leds-ams-delta.c
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/backlight.h>
#include <linux/notifier.h>
#define DRVNAME "sunlight"

struct mutex sunlight_lock;
struct platform_device *pdev = NULL;
extern struct  backlight_device *s_backlight;
extern int sunlight_flag;
extern int sunlight_set_sde_backlight(struct backlight_device *bd);
static void sunlight_led_set(struct led_classdev *led_cdev,
		enum led_brightness value)
{
	mutex_lock(&sunlight_lock);
	if(s_backlight)
	{
	printk("sunlight_led_set value=%d\n",value);
		if(value == 1){
			printk("Boost mode enable...\n");
			sunlight_flag = 1;
  		}else if(value == 6){
			printk("DP connected mode...\n");
			sunlight_flag = 6;
		}else if(value == 7){
			printk("DP disconnected mode...\n");
			sunlight_flag = 7;
		}else{  
			printk("Boost mode disable...\n");
			sunlight_flag = 0;
  		}		
		sunlight_set_sde_backlight(s_backlight);
	}	
	mutex_unlock(&sunlight_lock);
}

static struct led_classdev sunlight_led = {
	.name		= "sunlight_led",
	.brightness_set	= sunlight_led_set,
	.flags		= LED_CORE_SUSPENDRESUME,
};

static int sunlight_led_probe(struct platform_device *pdev)
{
	int ret ;
	mutex_init(&sunlight_lock);
	ret = devm_led_classdev_register(&pdev->dev, &sunlight_led);
	if(ret < 0){
		dev_err(&pdev->dev, "can't register LED %s\n", sunlight_led.name);
  		mutex_destroy(&sunlight_lock);
	}
	return ret;
}

static struct platform_driver sunlight_led_driver = {
	.probe		= sunlight_led_probe,
	.driver		= {
		.name		= DRVNAME,
	},
};


int sunlight_led_init(void)
{
	int ret;
	ret = platform_driver_register(&sunlight_led_driver);
	if (ret < 0)
  		goto out;

  	pdev = platform_device_register_simple(DRVNAME, -1, NULL, 0);
  	if (IS_ERR(pdev)) {
  		ret = PTR_ERR(pdev);
  		platform_driver_unregister(&sunlight_led_driver);
  		goto out;
  	}

  out:
	return ret;
}

void sunlight_led_exit(void)
{
	platform_device_unregister(pdev);
	platform_driver_unregister(&sunlight_led_driver);
}

EXPORT_SYMBOL(sunlight_led_init);
EXPORT_SYMBOL(sunlight_led_exit);

/*module_init(sunlight_led_init);
module_exit(sunlight_led_exit);

MODULE_AUTHOR("Keira Li");
MODULE_DESCRIPTION("sunlight led driver");
MODULE_LICENSE("GPL");*/

