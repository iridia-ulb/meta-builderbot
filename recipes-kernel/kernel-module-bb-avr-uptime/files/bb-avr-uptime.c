// SPDX-License-Identifier: GPL-2.0+

/*
 * Test module for the BuilderBot AVR multifunction core driver.
 *
 * Copyright (C) 2018 Michael Allwright
 */


#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/mfd/bb-avr.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>

static const struct of_device_id bb_avr_uptime_of_match[] = {
	{ .compatible = "ulb,bb-avr-uptime", },
	{ /* sentinel */ }
};

/**
 * struct bb_avr_uptime - AVR BuilderBot Uptime module
 *
 * @avr: Pointer to parent BuilderBot AVR device
 */
struct bb_avr_uptime {
	struct bb_avr *avr;
	struct delayed_work uptime_work;
	struct platform_device *pdev;
};

static void bb_avr_uptime_work(struct work_struct *work)
{
	unsigned char rx_data[4];

	struct bb_avr_uptime *avr_uptime =
		container_of(work, struct bb_avr_uptime, uptime_work.work);
	
	bb_avr_exec(avr_uptime->avr, BB_AVR_CMD_GET_UPTIME, NULL, 0, rx_data, 4);

	dev_err(&avr_uptime->pdev->dev, "%02x %02x %02x %02x",
		rx_data[0], rx_data[1], rx_data[2], rx_data[3]);

	schedule_delayed_work(&avr_uptime->uptime_work, 5 * HZ);
}

static int bb_avr_uptime_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bb_avr_uptime *avr_uptime;
	int ret = 0;

	avr_uptime = devm_kzalloc(dev, sizeof(*avr_uptime), GFP_KERNEL);
	if (!avr_uptime)
		return -ENOMEM;

	avr_uptime->pdev = pdev;

	/* get the parent AVR device */
	avr_uptime->avr = dev_get_drvdata(dev->parent);
	/* set drvdata so we can access it in bb_avr_uptime_remove */
 	dev_set_drvdata(dev, avr_uptime);

	INIT_DELAYED_WORK(&avr_uptime->uptime_work, bb_avr_uptime_work);

	schedule_delayed_work(&avr_uptime->uptime_work, 5 * HZ);

	return ret;
}

static int bb_avr_uptime_remove(struct platform_device *pdev) {
	struct bb_avr_uptime *avr_uptime = dev_get_drvdata(&pdev->dev);

	cancel_delayed_work(&avr_uptime->uptime_work);
	flush_scheduled_work();

	return 0;
}

static struct platform_driver bb_avr_uptime_driver = {
	.probe = bb_avr_uptime_probe,
	.remove = bb_avr_uptime_remove,
	.driver = {
		.name = "bb-avr-uptime",
		.of_match_table = bb_avr_uptime_of_match,
	},
};

module_platform_driver(bb_avr_uptime_driver);

MODULE_DEVICE_TABLE(of, bb_avr_uptime_of_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR Uptime");
//MODULE_ALIAS("platform:bb-avr-uptime");
