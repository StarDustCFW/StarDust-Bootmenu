#include "minerva/minerva.h"

#include "utils/types.h"
#include "utils/util.h"
#include "menu/tools/fs_utils.h"

#include "soc/fuse.h"
#include "soc/t210.h"
#include "soc/clock.h"

#include "gfx/gfx.h"

#include "mem/sdram.h"

#include "ianos/ianos.h"

#include "../modules/minerva/mtc.h"

#define MTC_INIT_MAGIC 0x3043544D
#define MTC_NEW_MAGIC  0x5243544D

char *path="StarDust/sys/minerva.bso";
/*

StarDust/sys/libsys_minerva.bso
*/
static u32 get_current_freq(mtc_config_t mtc_cfg)
{
    // Get current frequency and improve it to 800000
	for (u32 curr_ram_idx = 0; curr_ram_idx < 10; curr_ram_idx++)
	{
		if (CLOCK(CLK_RST_CONTROLLER_CLK_SOURCE_EMC) == mtc_cfg.mtc_table[curr_ram_idx].clk_src_emc)
			return mtc_cfg.mtc_table[curr_ram_idx].rate_khz;
	}
    return 0;
}

/**
 * @brief Increase switch clock's frequency from <from> to <to> using minerva training module
 * 
 * @param mtc_cfg Minerva config
 * @param from Initial frequency
 * @param to Final freq
 * @param train_mode Type of training mode
 */
static u32 increase_freq(mtc_config_t* mtc_cfg, u32 from, u32 to, enum train_mode_t train_mode)
{
    if (from != 0)
        mtc_cfg->rate_from = from;
	
    if (to != 0)
        mtc_cfg->rate_to = to;
	
    mtc_cfg->train_mode = train_mode;

    gfx_printf(&g_gfx_con, "Training and switching %7d -> %7d\n", mtc_cfg->rate_from, mtc_cfg->rate_to);
	return ianos_loader(false, path, DRAM_LIB, (void *)mtc_cfg);
}

void minerva()
{
    mtc_config_t mtc_cfg;

    /* Mount sd card */
    if(!sd_mount())
    {
        gfx_printf(&g_gfx_con, "Error mounting sd card\n");
        return;
    }

	gfx_printf(&g_gfx_con, "-- Minerva Training Cell --\n");
	
    // Set table to ram.
	mtc_cfg.mtc_table = NULL;
	mtc_cfg.sdram_id = get_sdram_id();
	mtc_cfg.init_done = MTC_NEW_MAGIC;
	gfx_printf(&g_gfx_con, "RamID %d..\n",mtc_cfg.sdram_id);
	if (ianos_loader(false, path, DRAM_LIB, (void *)&mtc_cfg) != 0)
    {
        gfx_printf(&g_gfx_con, "Abort Minerva Training Cell\n");
        return;
    }

	gfx_printf(&g_gfx_con, "Starting training process.. \n");

    /* Get current frequency */
	u32 current_freq = get_current_freq(mtc_cfg);
    if (current_freq == 0)
    {
        gfx_printf(&g_gfx_con, "Error while getting current frequenciy %d..\n",mtc_cfg.sdram_id);
        return;
    }

	if (increase_freq(&mtc_cfg, current_freq, FREQ_800, OP_TRAIN_SWITCH) != 0)
    {
        gfx_printf(&g_gfx_con, "Abort Minerva Training Cell\n");
        return;
    }

    // The following frequency needs periodic training every 100ms.
	msleep(200);
	
	if (increase_freq(&mtc_cfg, 0, FREQ_1600, OP_PERIODIC_TRAIN) != 0)
    {
        gfx_printf(&g_gfx_con, "Abort Minerva Training Cell at periodic training\n");
        return;
    }

    /* Temperature compensation */
	if (increase_freq(&mtc_cfg, 0, 0, OP_TEMP_COMP) != 0)
    {
        gfx_printf(&g_gfx_con, "Abort Minerva Training Cell at temperature compensation\n");
        return;
    }

	gfx_printf(&g_gfx_con, "-- Finished Minerva Training Cell --\n");
}
