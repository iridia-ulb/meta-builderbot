// SPDX-License-Identifier: GPL-2.0+

/*
 * Test module for the BuilderBot AVR multifunction core driver.
 *
 * Copyright (C) 2018 Michael Allwright
 */


#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/mfd/bb-avr.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>

static const struct of_device_id bb_avr_i2c_of_match[] = {
	{
		.compatible = "ulb,bb-avr-i2c",
	},
	{ /* sentinel */ }
};

/**
 * struct bb_avr_i2c - BuilderBot AVR I2C Remote Bus
 *
 * @avr:	Pointer to parent BuilderBot AVR device
 */
struct bb_avr_i2c_dev {
	struct bb_avr *avr;
   struct i2c_adapter adapter;
   struct platform_device *pdev;
};

static s32 bb_avr_smbus_xfer(struct i2c_adapter *adapter, u16 addr,
                             unsigned short flags, char read_write, u8 command,
                             int protocol, union i2c_smbus_data *data)
{
   struct bb_avr_i2c_dev *dev = i2c_get_adapdata(adapter);
   int ret = 0;
   u8 tx_data[3];
   /* device address */
   tx_data[0] = (addr & 0x7f);
   /* select the transfer protocol */
   switch(protocol) {
   case I2C_SMBUS_BYTE:
      if(read_write == I2C_SMBUS_READ) {
         ret = bb_avr_exec(dev->avr, BB_AVR_CMD_READ_SMBUS_BYTE,
               		      tx_data, 1, &data->byte, 1);
      }
      else {
         /* command is the byte to write */
         tx_data[1] = command;
         ret = bb_avr_exec(dev->avr, BB_AVR_CMD_WRITE_SMBUS_BYTE,
               		      tx_data, 2, NULL, 0);
      }
      break;
   case I2C_SMBUS_BYTE_DATA:
      if(read_write == I2C_SMBUS_READ) {
         /* command is the source register */
         tx_data[1] = command;
         ret = bb_avr_exec(dev->avr, BB_AVR_CMD_READ_SMBUS_BYTE_DATA,
               		      tx_data, 2, &data->byte, 1);
      }
      else {
         /* command is the destination register */
         tx_data[1] = command;
         tx_data[2] = data->byte;
         ret = bb_avr_exec(dev->avr, BB_AVR_CMD_WRITE_SMBUS_BYTE_DATA,
               		      tx_data, 3, NULL, 0);
      }
      break;
   case I2C_SMBUS_WORD_DATA:
      if(read_write == I2C_SMBUS_READ) {
         /* command is the source register */
         tx_data[1] = command;
         ret = bb_avr_exec(dev->avr, BB_AVR_CMD_READ_SMBUS_WORD_DATA,
               		      tx_data, 2, &data->word, 2);
      }
      else {
         return -EOPNOTSUPP;
      }
      break;
   case I2C_SMBUS_I2C_BLOCK_DATA:
      if(read_write == I2C_SMBUS_READ) {
         /* command is the source register */
         tx_data[1] = command;
         tx_data[2] = data->block[0];
         ret = bb_avr_exec(dev->avr, BB_AVR_CMD_READ_SMBUS_I2C_BLOCK_DATA,
               		      tx_data, 3, &data->block[1], data->block[0]);
      }
      else {
         return -EOPNOTSUPP;
      }
      break;
   default:
      return -EOPNOTSUPP;
      break;
   }
   return ret;
}

static u32 bb_avr_i2c_func(struct i2c_adapter *adapter)
{
	return (I2C_FUNC_SMBUS_BYTE |
           I2C_FUNC_SMBUS_BYTE_DATA |
           I2C_FUNC_SMBUS_READ_WORD_DATA |
           I2C_FUNC_SMBUS_READ_I2C_BLOCK);
}

static struct i2c_algorithm bb_avr_i2c_algorithm = {
//	.master_xfer	= bb_avr_i2c_rw,
   .smbus_xfer = bb_avr_smbus_xfer,
	.functionality	= bb_avr_i2c_func,
};

static int bb_avr_i2c_probe(struct platform_device *pdev)
{
   struct bb_avr_i2c_dev *dev;
	dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;
   /* configure the adapter */
   i2c_set_adapdata(&dev->adapter, dev);
   dev->adapter.owner = THIS_MODULE;
   dev->adapter.class = I2C_CLASS_DEPRECATED;
	strlcpy(dev->adapter.name, "Builderbot AVR I2C adapter", sizeof(dev->adapter.name));
	dev->adapter.algo = &bb_avr_i2c_algorithm;
	dev->adapter.dev.parent = &(pdev->dev);
	dev->adapter.dev.of_node = pdev->dev.of_node;
   /* get the parent avr device */
	dev->avr = dev_get_drvdata(pdev->dev.parent);

   /* set our drvdata so we can access it in bb_avr_i2c_remove */
 	//dev_set_drvdata(&pdev->dev, dev);

	return i2c_add_adapter(&dev->adapter);
}

/*
static int bb_avr_i2c_remove(struct platform_device *pdev) {
   struct bb_avr_i2c *avr_i2c = dev_get_drvdata(&pdev->dev);
   return 0;
}
*/

static struct platform_driver bb_avr_i2c_driver = {
	.probe = bb_avr_i2c_probe,
//	.remove = bb_avr_i2c_remove,
	.driver = {
		.name = "bb-avr-i2c",
		.of_match_table = bb_avr_i2c_of_match,
	},
};

module_platform_driver(bb_avr_i2c_driver);

MODULE_DEVICE_TABLE(of, bb_avr_i2c_of_match);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_DESCRIPTION("BuilderBot AVR Remote I2C Bus");
//MODULE_ALIAS("platform:bb-avr-i2c");
