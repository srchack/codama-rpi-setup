/*
 * driver for codama Mic Array compatible hardware
 *
 * Author:	Yuhei OKAWA <tochiro.srchack@gmail.com>
 *		Copyright 2019
 * Based on googlevoicehat-codec.c by Peter Malkin
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

#include <linux/device.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>
#include <sound/soc-dapm.h>

static int codama_codec_probe(struct snd_soc_codec *codec) {
  return 0;
}

static int codama_codec_remove(struct snd_soc_codec *codec) {
  return 0;
}

static const struct snd_soc_dapm_widget codama_dapm_widgets[] = {
};

static const struct snd_soc_dapm_route codama_dapm_routes[] = {
};

static struct snd_soc_codec_driver codama_codec_driver = {
    .probe = codama_codec_probe,
    .remove = codama_codec_remove,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    .component_driver = {
#endif
    .dapm_widgets = codama_dapm_widgets,
    .num_dapm_widgets = ARRAY_SIZE(codama_dapm_widgets),
    .dapm_routes = codama_dapm_routes,
    .num_dapm_routes = ARRAY_SIZE(codama_dapm_routes),
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    },
#endif
};

static struct snd_soc_dai_driver codama_dai = {
    .name = "codama-hifi",
    .playback = {.stream_name = "Playback",
                 .channels_min = 1,
                 .channels_max = 2,
                 .rates = (SNDRV_PCM_RATE_8000|SNDRV_PCM_RATE_11025|SNDRV_PCM_RATE_16000),
                 .formats = (SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_U16_LE | SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_U24_LE | SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_U32_LE | SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_LE)},
    .capture = {.stream_name = "Capture",
                .channels_min = 1,
                .channels_max = 6,
                .rates = (SNDRV_PCM_RATE_8000|SNDRV_PCM_RATE_11025|SNDRV_PCM_RATE_16000),
                .formats = (SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_U16_LE | SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_U24_LE | SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_U32_LE | SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_LE)},
    .symmetric_rates = 1};

#ifdef CONFIG_OF
static const struct of_device_id codama_ids[] = {
    {
        .compatible = "codama,codama",
    },
    {}};
MODULE_DEVICE_TABLE(of, codama_ids);
#endif

static int codama_platform_probe(struct platform_device *pdev) {
  return snd_soc_register_codec(&pdev->dev, &codama_codec_driver, &codama_dai, 1);
}

static int codama_platform_remove(struct platform_device *pdev) {
  snd_soc_unregister_codec(&pdev->dev);
  return 0;
}

static struct platform_driver codama_driver = {
    .driver =
        {
            .name = "codama-codec", .of_match_table = of_match_ptr(codama_ids),
        },
    .probe = codama_platform_probe,
    .remove = codama_platform_remove,
};

module_platform_driver(codama_driver);

MODULE_DESCRIPTION("Simple I2S Hifiberry & ICS43432 MEMS Codec");
MODULE_AUTHOR("Greg Miell <greg@gothack.ninja>");
MODULE_LICENSE("GPL v2");
