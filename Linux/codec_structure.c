// Sample Driver for Codec
/*
 * Codec driver
 * References :
 * 1. lm4857.c
 * 2. MAX9768.c
 * 3. max9850.c
 * 
 * PCM5102 - LRCK (pin 15), BCK (pin 13), and DIN (pin 14)
 * Data formats are selected using the FMT (pin 16) -to be set Low for I2S
 * Author: Sandeep A K
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#include <sound/core.h>
#include <sound/soc.h>
#include <sound/tlv.h>

/*Register specific to Codec that we are using
Array of {reg, val} with default values*/
static const struct reg_default codec_default_regs[] = {
	{ 0x0, 0x00 },//###
	{ 0x1, 0x00 },//###
	{ 0x2, 0x00 },//###
	{ 0x3, 0x00 },//###
};

static const struct snd_kcontrol_new codec_controls[] = {
    //###
};

/*DAPM (Dynamic Audio Power Management) - defines audio paths and power domains.*/
static const struct snd_soc_dapm_widget codec_dapm_widgets[] = {
    //###
};

static const struct snd_soc_dapm_route codec_routes[] = {
    //###
};


static const struct snd_soc_component_driver codec_component_driver = {
    .controls = codec_controls,
    .num_controls = ARRAY_SIZE(codec_controls),
    .dapm_widgets = codec_dapm_widgets,
    .num_dapm_widgets = ARRAY_SIZE(codec_dapm_widgets),
    .dapm_routes = codec_routes,
    .num_dapm_routes = ARRAY_SIZE(codec_routes),

};

static const struct regmap_config codec_regmap_config = {
    .val_bits = 6, // value bits
    .reg_bits = 2, //Address bits

    .max_regiter = ,//### Highest Register address

    .cache_type = ,//### REGCACHE_RBTREE or REGCACHE_FLAT cache type
    .reg_defaults = codec_default_regs, //Array of {reg, val} with default values
    .num_reg_defaults = ARRAY_SIZE(codec_default_regs),
};

static int codec_i2c_probe(struct i2c_client *i2c)
{
    struct regmap *regmap;
    regmap = devm_regmap_init_i2c(i2c, &codec_regmap_config);
    if(IS_ERR(regmap))
        return PTR_ERR(regmap);

    return devm_snd_soc_register_component(&i2c->dev,&codec_component_driver,NULL,0);
}

static const struct i2c_device_id codec_i2c_id[] = {
	{ "sdak_codec" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, codec_i2c_id);

static struct i2c_driver sample_codec_i2c_driver = {
	.driver = {
		.name = "sdak_codec",
	},
	.probe = codec_i2c_probe,
	.id_table = codec_i2c_id,
};

module_i2c_driver(sample_codec_i2c_driver);

MODULE_AUTHOR("Sandeep A K");
MODULE_DESCRIPTION("Sample Codec driver");
MODULE_LICENSE("GPL");