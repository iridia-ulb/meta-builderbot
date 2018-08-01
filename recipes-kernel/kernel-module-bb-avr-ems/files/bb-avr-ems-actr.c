// SPDX-License-Identifier: GPL-2.0+

/*
 * Electromagnet system actuator driver for the BuilderBot
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
#include <linux/iio/buffer.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

static const struct of_device_id bb_avr_ems_of_match[] = {
	{ .compatible = "ulb,bb-avr-ems-actr" },
	{ /* sentinel */ }
};

/**
 * struct bb_avr_ems - Electromagnet system actuator
 *
 * @avr: Pointer to parent BuilderBot AVR device
 */
struct bb_avr_ems {
	struct bb_avr *avr;
};

static const struct iio_info bb_avr_ems_info = {};

static int bb_avr_ems_buffer_preenable(struct iio_dev *indio_dev)
{
	struct bb_avr_ems *ems = iio_priv(indio_dev);

	u8 enable = 1;

	return bb_avr_exec(ems->avr, BB_AVR_CMD_SET_EM_CHARGE_MODE,
			   &enable, 1, NULL, 0);
}

static int bb_avr_ems_buffer_postdisable(struct iio_dev *indio_dev)
{
	struct bb_avr_ems *ems = iio_priv(indio_dev);

	u8 enable = 0;

	return bb_avr_exec(ems->avr, BB_AVR_CMD_SET_EM_CHARGE_MODE,
			   &enable, 1, NULL, 0);
}

static const struct iio_buffer_setup_ops bb_avr_ems_buffer_setup_ops = {
	.preenable   = bb_avr_ems_buffer_preenable,
	.postenable  = iio_triggered_buffer_postenable,
	.predisable  = iio_triggered_buffer_predisable,
	.postdisable = bb_avr_ems_buffer_postdisable,
};

static const struct iio_chan_spec bb_avr_ems_channels[] = {
{
		.type = IIO_INDEX,
		.indexed = true,
		.channel = 0,
		.output = true,
		.scan_index = 0,
		.scan_type = {
			.sign = 'u',
			.realbits = 2,
			.storagebits = 8,
			.endianness = IIO_BE,
		},
	}
};

static const unsigned long bb_avr_ems_scan_masks[] = {0x1, 0};

static irqreturn_t bb_avr_ems_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct bb_avr_ems *ems = iio_priv(indio_dev);
	struct iio_buffer *buffer = indio_dev->buffer;

	int ret;
	u8 tx_data;

	ret = iio_buffer_remove_sample(buffer, &tx_data);

	if (ret < 0) {
		if(ret != -ENODATA)
			dev_err(&indio_dev->dev,
				"iio_buffer_remove_sample failed: %d", ret);
		goto out;
	}

	bb_avr_exec(ems->avr, BB_AVR_CMD_SET_EM_DISCHARGE_MODE,
		    &tx_data, 1, NULL, 0);
out:
	iio_trigger_notify_done(indio_dev->trig);
	return IRQ_HANDLED;
}


static int bb_avr_ems_probe(struct platform_device *pdev)
{
	struct iio_dev *indio_dev;
	struct bb_avr_ems *ems;
	int ret = 0;

	indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(*ems));
	if (!indio_dev)
		return -ENOMEM;
	ems = iio_priv(indio_dev);
	/* set the parent AVR device */
	ems->avr = dev_get_drvdata(pdev->dev.parent);

	/* set up the indio_dev struct */
	dev_set_drvdata(&pdev->dev, indio_dev);
	indio_dev->name = "ems-actr";
	indio_dev->dev.parent = &pdev->dev;
	indio_dev->info = &bb_avr_ems_info;
	indio_dev->direction = IIO_DEVICE_DIRECTION_OUT;
	indio_dev->modes = INDIO_BUFFER_SOFTWARE;
	indio_dev->channels = bb_avr_ems_channels;
	indio_dev->num_channels = ARRAY_SIZE(bb_avr_ems_channels);
	indio_dev->available_scan_masks = bb_avr_ems_scan_masks;

	ret = iio_triggered_buffer_setup(indio_dev, NULL,
					 bb_avr_ems_trigger_handler,
					 &bb_avr_ems_buffer_setup_ops);
	if(ret < 0)
		goto err_out;
	ret = iio_device_register(indio_dev);
err_out:
	return ret;
}

static int bb_avr_ems_remove(struct platform_device *pdev)
{
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);

	iio_triggered_buffer_cleanup(indio_dev);
	iio_device_unregister(indio_dev);

	return 0;
}

static struct platform_driver bb_avr_ems_driver = {
	.probe = bb_avr_ems_probe,
	.remove = bb_avr_ems_remove,
	.driver = {
		.name = "bb-avr-ems-actr",
		.of_match_table = bb_avr_ems_of_match,
	},
};

module_platform_driver(bb_avr_ems_driver);

MODULE_DEVICE_TABLE(of, bb_avr_ems_of_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR Electromagnet System Actuator");
