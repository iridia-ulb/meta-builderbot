// SPDX-License-Identifier: GPL-2.0+

/*
 * Lift actuator system actuator driver for the BuilderBot
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

static const struct of_device_id bb_avr_las_of_match[] = {
	{ .compatible = "ulb,bb-avr-las-actuator" },
	{ /* sentinel */ }
};

/**
 * struct bb_avr_las - Lift actuator system actuator
 *
 * @avr: Pointer to parent BuilderBot AVR device
 */
struct bb_avr_las {
	struct bb_avr *avr;
};

static int bb_avr_las_read_raw(struct iio_dev *indio_dev,
                               struct iio_chan_spec const *chan,
			       int *val, int *val2, long m)
{
	return -ENOSYS;
}

static int bb_avr_las_write_raw(struct iio_dev *indio_dev,
				struct iio_chan_spec const *chan,
				int val, int val2, long mask)
{
	return -ENOSYS;
}

static const struct iio_info bb_avr_las_info = {
	.read_raw = bb_avr_las_read_raw,
	.write_raw = bb_avr_las_write_raw,
};

static ssize_t bb_avr_las_write_calibrate(struct iio_dev *indio_dev,
				       uintptr_t private,
				       struct iio_chan_spec const *ch,
				       const char *buf, size_t len)
{
	struct bb_avr_las *las = iio_priv(indio_dev);
	bb_avr_exec(las->avr, BB_AVR_CMD_CALIBRATE_LIFT_ACTUATOR,
		    NULL, 0, NULL, 0);
	return len;
}

static ssize_t bb_avr_las_write_emergency_stop(struct iio_dev *indio_dev,
				       uintptr_t private,
				       struct iio_chan_spec const *ch,
				       const char *buf, size_t len)
{
	struct bb_avr_las *las = iio_priv(indio_dev);
	bb_avr_exec(las->avr, BB_AVR_CMD_EMER_STOP_LIFT_ACTUATOR,
		    NULL, 0, NULL, 0);
	return len;
}

static const struct iio_chan_spec_ext_info bb_avr_las_ext_info[] = {
	{
		.name = "calibrate",
		.write = bb_avr_las_write_calibrate,
		.shared = IIO_SHARED_BY_ALL,
	}, {
		.name = "emergency_stop",
		.write = bb_avr_las_write_emergency_stop,
		.shared = IIO_SHARED_BY_ALL,
	},
};

static const struct iio_chan_spec bb_avr_las_channels[] = {
  {
		.type = IIO_DISTANCE,
		.indexed = false,
		.channel = 0,
		.output = true,
		.ext_info = bb_avr_las_ext_info,
		.scan_index = 0,
		.scan_type = {
			.sign = 'u',
			.realbits = 8,
			.storagebits = 8,
			.endianness = IIO_BE,
		},
	}
};

static const unsigned long bb_avr_las_scan_masks[] = {0x1, 0};

static irqreturn_t bb_avr_las_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct bb_avr_las *las = iio_priv(indio_dev);
	struct iio_buffer *buffer = indio_dev->buffer;

	int ret;
	u8 position;

	ret = iio_buffer_remove_sample(buffer, &position);

	if (ret < 0) {
		dev_err(&indio_dev->dev,
			"iio_buffer_remove_sample failed: %d", ret);
		goto out;
	}

	bb_avr_exec(las->avr, BB_AVR_CMD_SET_LIFT_ACTUATOR_POSITION,
		    &position, 1, NULL, 0);
out:
	iio_trigger_notify_done(indio_dev->trig);
	return IRQ_HANDLED;
}



static int bb_avr_las_probe(struct platform_device *pdev)
{
	struct iio_dev *indio_dev;
	struct bb_avr_las *las;
	int ret = 0;

	indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(*las));
	if (!indio_dev)
		return -ENOMEM;
	las = iio_priv(indio_dev);
	/* set the parent AVR device */
	las->avr = dev_get_drvdata(pdev->dev.parent);

	/* set up the indio_dev struct */
	dev_set_drvdata(&pdev->dev, indio_dev);
	indio_dev->name = "las-actuator";
	indio_dev->dev.parent = &pdev->dev;
	indio_dev->info = &bb_avr_las_info;
	indio_dev->modes = INDIO_BUFFER_SOFTWARE;
	indio_dev->channels = bb_avr_las_channels;
	indio_dev->num_channels = ARRAY_SIZE(bb_avr_las_channels);
	
	ret = iio_triggered_buffer_setup(indio_dev, NULL,
					 bb_avr_las_trigger_handler, NULL);
	if(ret < 0)
		goto err_out;
	ret = iio_device_register(indio_dev);
err_out:
	return ret;
}

static int bb_avr_las_remove(struct platform_device *pdev)
{
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);

	iio_triggered_buffer_cleanup(indio_dev);
	iio_device_unregister(indio_dev);

	return 0;
}

static struct platform_driver bb_avr_las_driver = {
	.probe = bb_avr_las_probe,
	.remove = bb_avr_las_remove,
	.driver = {
		.name = "bb-avr-las-actuator",
		.of_match_table = bb_avr_las_of_match,
	},
};

module_platform_driver(bb_avr_las_driver);

MODULE_DEVICE_TABLE(of, bb_avr_las_of_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR Differential Drive System");
