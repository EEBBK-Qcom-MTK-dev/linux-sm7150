// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of_graph.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct boe_tv127xum_lb0_5sp0 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi[2];		/* [0]=primary, [1]=secondary */
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
};

static const struct regulator_bulk_data boe_tv127xum_lb0_5sp0_supplies[] = {
	{ .supply = "vdd3p3" },
	{ .supply = "vddio" },
};

static inline struct boe_tv127xum_lb0_5sp0 *to_boe_tv127xum_lb0_5sp0(struct drm_panel *panel)
{
	return container_of_const(panel, struct boe_tv127xum_lb0_5sp0, panel);
}

static void boe_tv127xum_lb0_5sp0_reset(struct boe_tv127xum_lb0_5sp0 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int boe_tv127xum_lb0_5sp0_on(struct boe_tv127xum_lb0_5sp0 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi[0];
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };

	/* 命令只通过主 DSI 发送 */
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x99, 0x95, 0x27);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_usleep_range(&dsi_ctx, 1000, 2000);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x99, 0x00, 0x00);

	return dsi_ctx.accum_err;
}

static int boe_tv127xum_lb0_5sp0_off(struct boe_tv127xum_lb0_5sp0 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi[0];
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x99, 0x95, 0x27);
	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_usleep_range(&dsi_ctx, 1000, 2000);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int boe_tv127xum_lb0_5sp0_prepare(struct drm_panel *panel)
{
	struct boe_tv127xum_lb0_5sp0 *ctx = to_boe_tv127xum_lb0_5sp0(panel);
	struct device *dev = &ctx->dsi[0]->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(boe_tv127xum_lb0_5sp0_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	boe_tv127xum_lb0_5sp0_reset(ctx);

	ret = boe_tv127xum_lb0_5sp0_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(boe_tv127xum_lb0_5sp0_supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int boe_tv127xum_lb0_5sp0_unprepare(struct drm_panel *panel)
{
	struct boe_tv127xum_lb0_5sp0 *ctx = to_boe_tv127xum_lb0_5sp0(panel);
	struct device *dev = &ctx->dsi[0]->dev;
	int ret;

	ret = boe_tv127xum_lb0_5sp0_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(boe_tv127xum_lb0_5sp0_supplies), ctx->supplies);

	return 0;
}

/* Dual DSI: 水平分辨率翻倍，所有水平时序参数也翻倍 */
static const struct drm_display_mode boe_tv127xum_lb0_5sp0_mode = {
	.clock = (1600 + 12 + 10 + 32) * (2176 + 255 + 6 + 34) * 60 / 1000,
	.hdisplay = 1600,
	.hsync_start = 1600 + 12,
	.hsync_end = 1600 + 12 + 10,
	.htotal = 1600 + 12 + 10 + 32,
	.vdisplay = 2176,
	.vsync_start = 2176 + 255,
	.vsync_end = 2176 + 255 + 6,
	.vtotal = 2176 + 255 + 6 + 34,
	.width_mm = 294,	/* 物理宽度也应翻倍？通常不变，但若面板实际宽度对应1600则需调整，此处按比例假设 */
	.height_mm = 197,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int boe_tv127xum_lb0_5sp0_get_modes(struct drm_panel *panel,
				struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &boe_tv127xum_lb0_5sp0_mode);
}

static const struct drm_panel_funcs boe_tv127xum_lb0_5sp0_panel_funcs = {
	.prepare = boe_tv127xum_lb0_5sp0_prepare,
	.unprepare = boe_tv127xum_lb0_5sp0_unprepare,
	.get_modes = boe_tv127xum_lb0_5sp0_get_modes,
};

static int boe_tv127xum_lb0_5sp0_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct boe_tv127xum_lb0_5sp0 *ctx;
	struct mipi_dsi_host *dsi_sec_host;
	struct device_node *dsi_sec;
	int ret, i;

	ctx = devm_drm_panel_alloc(dev, struct boe_tv127xum_lb0_5sp0, panel,
				   &boe_tv127xum_lb0_5sp0_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(boe_tv127xum_lb0_5sp0_supplies),
					    boe_tv127xum_lb0_5sp0_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	/* 获取第二个 DSI 端口 */
	dsi_sec = of_graph_get_remote_node(dsi->dev.of_node, 1, -1);
	if (dsi_sec) {
		const struct mipi_dsi_device_info info = { "BOE TV127XUM LB0 5SP0 DSI1", 0,
							   dsi_sec };

		dsi_sec_host = of_find_mipi_dsi_host_by_node(dsi_sec);
		of_node_put(dsi_sec);
		if (!dsi_sec_host)
			return dev_err_probe(dev, -EPROBE_DEFER,
					     "Cannot get secondary DSI host\n");

		ctx->dsi[1] = devm_mipi_dsi_device_register_full(dev, dsi_sec_host, &info);
		if (IS_ERR(ctx->dsi[1]))
			return dev_err_probe(dev, PTR_ERR(ctx->dsi[1]),
					     "Cannot get secondary DSI node\n");

		mipi_dsi_set_drvdata(ctx->dsi[1], ctx);
	}

	ctx->dsi[0] = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	/* 配置两个 DSI 的参数 */
	for (i = 0; i < ARRAY_SIZE(ctx->dsi); i++) {
		if (!ctx->dsi[i])
			continue;

		ctx->dsi[i]->lanes = 4;
		ctx->dsi[i]->format = MIPI_DSI_FMT_RGB888;
		ctx->dsi[i]->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
					  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;
	}

	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	/* 依次 attach 两个 DSI */
	for (i = 0; i < ARRAY_SIZE(ctx->dsi); i++) {
		if (!ctx->dsi[i])
			continue;

		ret = mipi_dsi_attach(ctx->dsi[i]);
		if (ret < 0) {
			drm_panel_remove(&ctx->panel);
			return dev_err_probe(dev, ret,
					     "Failed to attach to DSI%d\n", i);
		}
	}

	return 0;
}

static void boe_tv127xum_lb0_5sp0_remove(struct mipi_dsi_device *dsi)
{
	struct boe_tv127xum_lb0_5sp0 *ctx = mipi_dsi_get_drvdata(dsi);
	int i, ret;

	/* 先 detach 所有 DSI（逆序） */
	for (i = ARRAY_SIZE(ctx->dsi) - 1; i >= 0; i--) {
		if (!ctx->dsi[i])
			continue;
		ret = mipi_dsi_detach(ctx->dsi[i]);
		if (ret < 0)
			dev_err(&ctx->dsi[i]->dev, "Failed to detach from DSI host: %d\n", ret);
	}

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id boe_tv127xum_lb0_5sp0_of_match[] = {
	{ .compatible = "boe,tv127xum-lb0-5sp0" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, boe_tv127xum_lb0_5sp0_of_match);

static struct mipi_dsi_driver boe_tv127xum_lb0_5sp0_driver = {
	.probe = boe_tv127xum_lb0_5sp0_probe,
	.remove = boe_tv127xum_lb0_5sp0_remove,
	.driver = {
		.name = "panel-boe-tv127xum-lb0-5sp0",
		.of_match_table = boe_tv127xum_lb0_5sp0_of_match,
	},
};
module_mipi_dsi_driver(boe_tv127xum_lb0_5sp0_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for Dual BOE TV127XUM-LB0-5SP0 video mode dsi panel");
MODULE_LICENSE("GPL");
