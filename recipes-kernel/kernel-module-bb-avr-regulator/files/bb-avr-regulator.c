// SPDX-License-Identifier: GPL-2.0+

/*
 * Test module for the BuilderBot AVR multifunction core driver.
 *
 * Copyright (C) 2018 Michael Allwright
 */

#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mfd/bb-avr.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/fixed.h>
#include <linux/regulator/machine.h>

#define SYSTEM_POWER_MICROVOLTS 3300000
#define ACTUATOR_POWER_MICROVOLTS 4000000
#define PM_STATUS_RESP_LEN 9
#define PM_STATUS_RESP_SYS_IND 0
#define PM_STATUS_RESP_ACT_IND 1

static const struct of_device_id bb_avr_regulator_of_match[] = {
	{
		.compatible = "ulb,bb-avr-regulator",
	},
	{ /* sentinel */ }
};

/**
 * struct bb_avr_regulator - AVR BuilderBot Regulator module
 *
 * @avr:	Pointer to parent BuilderBot AVR device
 */
struct bb_avr_regulator {
	struct bb_avr *avr;
   bool actuator_reg;
   struct regulator_desc desc;
	struct regulator_dev *dev;
};

static int bb_avr_regulator_is_enabled(struct regulator_dev *regulator) {
   struct bb_avr_regulator *drvdata = rdev_get_drvdata(regulator);
   int ret = 0;
   u8 rx_data[PM_STATUS_RESP_LEN];
   
   ret = bb_avr_exec(drvdata->avr, BB_AVR_CMD_GET_PM_STATUS,
                     NULL, 0, rx_data, ARRAY_SIZE(rx_data));
   if(ret < 0)
      return ret;

   return (drvdata->actuator_reg ?
      rx_data[PM_STATUS_RESP_ACT_IND] : 
      rx_data[PM_STATUS_RESP_SYS_IND]);
}

static int bb_avr_regulator_enable(struct regulator_dev *regulator)
{
   struct bb_avr_regulator *drvdata = rdev_get_drvdata(regulator);
   
   u8 enable = 1;
   u8 override = 3; // 150mA pass through
   
   if(drvdata->actuator_reg) {
      bb_avr_exec(drvdata->avr, BB_AVR_CMD_SET_ACTUATOR_INPUT_LIMIT_OVERRIDE,
                  &override, 1, NULL, 0);
      return bb_avr_exec(drvdata->avr, BB_AVR_CMD_SET_ACTUATOR_POWER_ENABLE,
	                      &enable, 1, NULL, 0);
   }
   else
      return -ENOSYS; /* system power command not yet implemented */
}

static int bb_avr_regulator_disable(struct regulator_dev *regulator)
{
   struct bb_avr_regulator *drvdata = rdev_get_drvdata(regulator);

   u8 enable = 0;
   
   if(drvdata->actuator_reg) {
      return bb_avr_exec(drvdata->avr, BB_AVR_CMD_SET_ACTUATOR_POWER_ENABLE,
	                      &enable, 1, NULL, 0);
   }
   else
      return -ENOSYS; /* system power command not yet implemented */

}

static struct regulator_ops bb_avr_regulator_ops = {
	.enable = bb_avr_regulator_enable,
	.disable = bb_avr_regulator_disable,
   .is_enabled = bb_avr_regulator_is_enabled,
};

static int bb_avr_regulator_probe(struct platform_device *pdev)
{
	struct bb_avr_regulator *drvdata;
   struct regulator_init_data *init_data;
   struct regulator_config cfg = { };
	int ret = 0;
   const char *domain = NULL;
   /* allocate memory for driver data */
	drvdata = devm_kzalloc(&pdev->dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;
	/* get the parent AVR device */
	drvdata->avr = dev_get_drvdata(pdev->dev.parent);
   /* read the regulator domain */
   ret = of_property_read_string(pdev->dev.of_node, "domain", &domain);
   if(ret < 0) {
      return ret;
   }
   if (!strcmp(domain, "system")) {
      drvdata->actuator_reg = false;
      drvdata->desc.n_voltages = 1;
      drvdata->desc.fixed_uV = SYSTEM_POWER_MICROVOLTS;
   }
   else if (!strcmp(domain, "actuators")) {
      drvdata->actuator_reg = true;
      drvdata->desc.n_voltages = 1;
      drvdata->desc.fixed_uV = ACTUATOR_POWER_MICROVOLTS;
   }
   else 
      return -EINVAL;

   init_data = 
      of_get_regulator_init_data(&pdev->dev,
                                 pdev->dev.of_node,
                                 &drvdata->desc);
	if (!init_data)
		return -EINVAL;

   drvdata->desc.name = devm_kstrdup(&pdev->dev, domain, GFP_KERNEL);
	drvdata->desc.type = REGULATOR_VOLTAGE;
	drvdata->desc.owner = THIS_MODULE;
	drvdata->desc.ops = &bb_avr_regulator_ops;

   cfg.dev = &pdev->dev;
	cfg.init_data = init_data;
	cfg.driver_data = drvdata;
	cfg.of_node = pdev->dev.of_node;

	drvdata->dev = 
      devm_regulator_register(&pdev->dev, &drvdata->desc, &cfg);
	if (IS_ERR(drvdata->dev)) {
		ret = PTR_ERR(drvdata->dev);
		dev_err(&pdev->dev, "Failed to register regulator: %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, drvdata);

	dev_info(&pdev->dev, "%s supplying %duV\n", drvdata->desc.name,
		drvdata->desc.fixed_uV);

	return ret;
}

static int bb_avr_regulator_remove(struct platform_device *pdev) {
	//struct bb_avr_regulator *avr_regulator = dev_get_drvdata(&pdev->dev);
	return 0;
}

static struct platform_driver bb_avr_regulator_driver = {
	.probe = bb_avr_regulator_probe,
	.remove = bb_avr_regulator_remove,
	.driver = {
		.name = "bb-avr-regulator",
		.of_match_table = bb_avr_regulator_of_match,
	},
};

module_platform_driver(bb_avr_regulator_driver);

MODULE_DEVICE_TABLE(of, bb_avr_regulator_of_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR Regulator");
//MODULE_ALIAS("platform:bb-avr-regulator");
