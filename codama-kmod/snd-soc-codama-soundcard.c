/*
 * driver for codama Mic Array compatible hardware
 *
 * Author: Yuhei OKAWA <tochiro.srchack@gmail.com>
 *         Copyright 2019
 * Based on googlevoicehat-soundcard.c by Peter Malkin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>

static int snd_rpi_codama_soundcard_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}

static int snd_rpi_codama_soundcard_hw_params(
	struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

	unsigned int sample_bits =
		snd_pcm_format_physical_width(params_format(params));

	return snd_soc_dai_set_bclk_ratio(cpu_dai, sample_bits * 2);
}

/* machine stream operations */
static struct snd_soc_ops snd_rpi_codama_soundcard_ops = {
	.hw_params = snd_rpi_codama_soundcard_hw_params,
};

static struct snd_soc_dai_link snd_rpi_codama_soundcard_dai[] = {
{
	.name		= "I2S codama Mic Array",
	.stream_name	= "I2S codama Mic Array",
	.cpu_dai_name	= "3f203000.i2s",
	.codec_dai_name	= "codama-hifi",
	.platform_name	= "3f203000.i2s",
	.codec_name	= "codama-codec",
	.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM,
	.ops		= &snd_rpi_codama_soundcard_ops,
	.init		= snd_rpi_codama_soundcard_init,
},
};

/* audio machine driver */
static struct snd_soc_card snd_rpi_codama_soundcard = {
	.name         = "snd_rpi_codama_soundcard",
	.owner        = THIS_MODULE,
	.dai_link     = snd_rpi_codama_soundcard_dai,
	.num_links    = ARRAY_SIZE(snd_rpi_codama_soundcard_dai),
};

static int snd_rpi_codama_soundcard_probe(struct platform_device *pdev)
{
	int ret = 0;

	snd_rpi_codama_soundcard.dev = &pdev->dev;

	if (pdev->dev.of_node) {
		struct device_node *i2s_node;
		struct snd_soc_dai_link *dai = &snd_rpi_codama_soundcard_dai[0];
		i2s_node = of_parse_phandle(pdev->dev.of_node,
					"i2s-controller", 0);

		if (i2s_node) {
			dai->cpu_dai_name = NULL;
			dai->cpu_of_node = i2s_node;
			dai->platform_name = NULL;
			dai->platform_of_node = i2s_node;
		}
	}

	ret = snd_soc_register_card(&snd_rpi_codama_soundcard);
	if (ret)
		dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n", ret);

	return ret;
}

static int snd_rpi_codama_soundcard_remove(struct platform_device *pdev)
{
	return snd_soc_unregister_card(&snd_rpi_codama_soundcard);
}

static const struct of_device_id snd_rpi_codama_soundcard_of_match[] = {
	{ .compatible = "codama,codama-soundcard", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_rpi_codama_soundcard_of_match);

static struct platform_driver snd_rpi_codama_soundcard_driver = {
	.driver = {
		.name   = "snd-codama-soundcard",
		.owner  = THIS_MODULE,
		.of_match_table = snd_rpi_codama_soundcard_of_match,
	},
	.probe          = snd_rpi_codama_soundcard_probe,
	.remove         = snd_rpi_codama_soundcard_remove,
};

module_platform_driver(snd_rpi_codama_soundcard_driver);

MODULE_DESCRIPTION("AoSC Driver for codama Mic Array");
MODULE_AUTHOR("Yuhei OKAWA <tochiro.srchack@gmail.com>");
MODULE_LICENSE("GPL v2");
