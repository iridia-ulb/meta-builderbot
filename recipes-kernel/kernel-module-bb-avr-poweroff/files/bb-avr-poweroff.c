/*
 * Requests the builderbot AVR power management system to power off
 *
 * Michael Allwright <allsey87@gmail.com>
 *
 * Copyright (C) 2018 Michael Allwright
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/cpu.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mfd/bb-avr.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/syscore_ops.h>

static const struct of_device_id bb_avr_poweroff_match[] = {
	{ .compatible = "ulb,bb-avr-poweroff", },
	{ /* sentinel */ }
};

/*
 * Hold configuration here, cannot be more than one instance of the driver
 * since pm_power_off itself is global.
 */
static struct bb_avr *avr;

static void bb_avr_poweroff_shutdown(void)
{
#ifdef CONFIG_PM_SLEEP_SMP
	int cpu = smp_processor_id();
	freeze_secondary_cpus(cpu);
#endif
}

static struct syscore_ops bb_avr_poweroff_syscore_ops = {
	.shutdown = bb_avr_poweroff_shutdown,
};

static void bb_avr_poweroff_do_poweroff(void)
{
	u8 enable = 0;
	/* Ensure the avr pointer is initialized */
	BUG_ON(!avr);
	/* Power down the actuator supply */
	bb_avr_exec(avr, BB_AVR_CMD_SET_ACTUATOR_POWER_ENABLE, &enable, 1, NULL, 0);
	usleep_range(2500, 10000);
	/* Power down the system supply */
	bb_avr_exec(avr, BB_AVR_CMD_SET_SYSTEM_POWER_ENABLE, &enable, 1, NULL, 0);
	/* Sleep while the power disappears */
	usleep_range(2500, 10000);
}

static int bb_avr_poweroff_probe(struct platform_device *pdev)
{
	/* If a pm_power_off function has already been added, leave it alone */
	if (pm_power_off != NULL) {
		dev_err(&pdev->dev,
			"%s: pm_power_off function already registered",
		       __func__);
		return -EBUSY;
	}

	avr = dev_get_drvdata(pdev->dev.parent);

	pm_power_off = &bb_avr_poweroff_do_poweroff;

	register_syscore_ops(&bb_avr_poweroff_syscore_ops);

	return 0;
}

static int bb_avr_poweroff_remove(struct platform_device *pdev)
{
	if (pm_power_off == &bb_avr_poweroff_do_poweroff)
		pm_power_off = NULL;

	return 0;
}

static struct platform_driver bb_avr_poweroff_driver = {
	.probe = bb_avr_poweroff_probe,
	.remove = bb_avr_poweroff_remove,
	.driver = {
		.name = "bb-avr-poweroff",
		.of_match_table = bb_avr_poweroff_match,
	},
};

module_platform_driver(bb_avr_poweroff_driver);

MODULE_DEVICE_TABLE(of, bb_avr_poweroff_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR Poweroff Driver");
//MODULE_ALIAS("platform:poweroff-bb-avr");
