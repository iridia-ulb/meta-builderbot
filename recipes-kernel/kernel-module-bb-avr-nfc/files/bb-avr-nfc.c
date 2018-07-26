// SPDX-License-Identifier: GPL-2.0+

/*
 * Test module for the BuilderBot AVR multifunction core driver.
 *
 * Copyright (C) 2018 Michael Allwright
 */


#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/mfd/bb-avr.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static const struct of_device_id bb_avr_nfc_of_match[] = {
	{ .compatible = "ulb,bb-avr-nfc", },
	{},
};

/**
 * struct bb_avr_nfc - AVR BuilderBot NFC module
 *
 * @avr: Pointer to parent BuilderBot AVR device
 */
struct bb_avr_nfc {
	struct platform_device *pdev;
	struct bb_avr *avr;
	struct miscdevice mdev;
};

static inline struct bb_avr_nfc *to_bb_avr_nfc(struct file *file)
{
	struct miscdevice *miscdev = file->private_data;

	return container_of(miscdev, struct bb_avr_nfc, mdev);
}

static ssize_t bb_avr_nfc_write(struct file *file, const char *buffer, size_t length,
				loff_t * off)
{
	int ret = 0;
	char *tx_buffer = NULL;  
	struct bb_avr_nfc *avr_nfc = to_bb_avr_nfc(file);

	tx_buffer = kzalloc(length, GFP_KERNEL);
	if (!tx_buffer) {
		return -ENOMEM;
	}
	copy_from_user(tx_buffer, buffer, length);

	ret = bb_avr_exec(avr_nfc->avr, BB_AVR_CMD_WRITE_NFC, tx_buffer, length, NULL, 0);

	kfree(tx_buffer);

	if(ret < 0)
		return ret;

	return length;
}

static struct file_operations bb_avr_nfc_fops = {
	.owner = THIS_MODULE,
	.write = bb_avr_nfc_write
};

static int bb_avr_nfc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bb_avr_nfc *avr_nfc;
	int ret = 0;

	avr_nfc = devm_kzalloc(&pdev->dev, sizeof(*avr_nfc), GFP_KERNEL);
	if (!avr_nfc) {
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, avr_nfc);
	avr_nfc->pdev = pdev;
	avr_nfc->avr = dev_get_drvdata(pdev->dev.parent);
	avr_nfc->mdev.minor  = MISC_DYNAMIC_MINOR;
	avr_nfc->mdev.name   = "nfc";
	avr_nfc->mdev.fops   = &bb_avr_nfc_fops;
	avr_nfc->mdev.parent = NULL;

	ret = misc_register(&avr_nfc->mdev);
	if (ret) {
	dev_err(&pdev->dev, "Failed to register misc dev\n");
	return ret;
	}
	return 0;
}

static int bb_avr_nfc_remove(struct platform_device *pdev) {
	struct bb_avr_nfc *avr_nfc = platform_get_drvdata(pdev);
	misc_deregister(&avr_nfc->mdev);
	return 0;
}

static struct platform_driver bb_avr_nfc_driver = {
	.probe = bb_avr_nfc_probe,
	.remove = bb_avr_nfc_remove,
	.driver = {
		.name = "bb-avr-nfc",
		.of_match_table = bb_avr_nfc_of_match,
	},
};

module_platform_driver(bb_avr_nfc_driver);

MODULE_DEVICE_TABLE(of, bb_avr_nfc_of_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR NFC");
//MODULE_ALIAS("platform:bb-avr-nfc");
