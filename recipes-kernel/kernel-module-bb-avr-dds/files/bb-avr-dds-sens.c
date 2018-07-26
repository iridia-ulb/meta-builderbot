// SPDX-License-Identifier: GPL-2.0+

/*
 * Differential drive system sensor driver for the BuilderBot
 *
 * Copyright (C) 2018 Michael Allwright
 */


#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/mfd/bb-avr.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/iio/buffer.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

static const struct of_device_id bb_avr_dds_of_match[] = {
	{ .compatible = "ulb,bb-avr-dds-sens" },
	{ /* sentinel */ }
};

/**
 * struct bb_avr_dds - Differential drive system actuator
 *
 * @avr: Pointer to parent BuilderBot AVR device
 */
struct bb_avr_dds {
	struct bb_avr *avr;
};

static const struct iio_info bb_avr_dds_info = {};

static const struct iio_chan_spec bb_avr_dds_channels[] = {
	{
		.type = IIO_ANGL_VEL,
		.indexed = true,
		.channel = 0,
		.scan_index = 0,
		.scan_type = {
			.sign = 's',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_BE,
		},
	}, {
		.type = IIO_ANGL_VEL,
		.indexed = true,
		.channel = 1,
		.scan_index = 1,
		.scan_type = {
			.sign = 's',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_BE,
		},
	},
	IIO_CHAN_SOFT_TIMESTAMP(2),
};

static const unsigned long bb_avr_dds_scan_masks[] = {0x3, 0};

static irqreturn_t bb_avr_dds_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct bb_avr_dds *dds = iio_priv(indio_dev);
	u8 rx_data[4];
	int ret;
	ret = bb_avr_exec(dds->avr, BB_AVR_CMD_GET_DDS_SPEED,
			  NULL, 0, rx_data, ARRAY_SIZE(rx_data));
	if (ret < 0)
		goto out;
	iio_push_to_buffers_with_timestamp(indio_dev, rx_data,
					   iio_get_time_ns(indio_dev));
out:
	iio_trigger_notify_done(indio_dev->trig);
	return IRQ_HANDLED;
}

static int bb_avr_dds_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev; // us
	struct iio_dev *indio_dev;
	struct bb_avr_dds *dds;
	int ret = 0;
	indio_dev = devm_iio_device_alloc(dev, sizeof(*dds));
	if (!indio_dev)
		return -ENOMEM;
	dds = iio_priv(indio_dev);
	/* set the parent AVR device */
	dds->avr = dev_get_drvdata(dev->parent);
	/* set up the indio_dev struct */
	dev_set_drvdata(&pdev->dev, indio_dev);
	indio_dev->name = "dds-sens";
	indio_dev->dev.parent = &pdev->dev;
	indio_dev->info = &bb_avr_dds_info;
	indio_dev->modes = INDIO_BUFFER_SOFTWARE;
	indio_dev->channels = bb_avr_dds_channels;
	indio_dev->num_channels = ARRAY_SIZE(bb_avr_dds_channels);
	indio_dev->available_scan_masks = bb_avr_dds_scan_masks;
	ret = iio_triggered_buffer_setup(indio_dev,
					 iio_pollfunc_store_time,
					 bb_avr_dds_trigger_handler,
					 NULL);
	if(ret < 0)
		goto err_out;
	ret = iio_device_register(indio_dev);
err_out:
	return ret;
}

static int bb_avr_dds_remove(struct platform_device *pdev)
{
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);

	iio_triggered_buffer_cleanup(indio_dev);
	iio_device_unregister(indio_dev);

	return 0;
}

static struct platform_driver bb_avr_dds_driver = {
	.probe = bb_avr_dds_probe,
	.remove = bb_avr_dds_remove,
	.driver = {
		.name = "bb-avr-dds-sens",
		.of_match_table = bb_avr_dds_of_match,
	},
};

module_platform_driver(bb_avr_dds_driver);

MODULE_DEVICE_TABLE(of, bb_avr_dds_of_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR Differential Drive System Sensor");
