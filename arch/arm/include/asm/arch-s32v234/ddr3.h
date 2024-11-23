/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2015-2016 Freescale Semiconductor, Inc.
 * (C) Copyright 2016-2017 NXP
 *
 */

#ifndef __ARCH_ARM_MACH_S32V234_DDR3_H__
#define __ARCH_ARM_MACH_S32V234_DDR3_H__

/* definitions for DDR3 PAD values */

#define DDR3_RESET_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DDR_INPUT_DIFF_DDR | \
	SIUL2_MSCR_DSE_34ohm |	\
	 SIUL2_MSCR_DDR_ODT_60ohm | SIUL2_MSCR_PUS_100K_UP)
#define DDR3_CLK0_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_CRPOINT_TRIM_1 | \
	 SIUL2_MSCR_DCYCLE_TRIM_NONE | SIUL2_MSCR_DDR_INPUT_DIFF_DDR | \
	SIUL2_MSCR_DDR_ODT_120ohm | \
	 SIUL2_MSCR_PUS_100K_UP)
#define DDR3_CAS_PAD		\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_ODT_60ohm | \
	 SIUL2_MSCR_DDR_INPUT_DIFF_DDR | SIUL2_MSCR_PUS_100K_UP)
#define DDR3_RAS_PAD		\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_ODT_60ohm | \
	 SIUL2_MSCR_DDR_INPUT_DIFF_DDR | SIUL2_MSCR_PUS_100K_UP)
#define DDR3_WE_B_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_ODT_60ohm | \
	 SIUL2_MSCR_DDR_INPUT_DIFF_DDR | SIUL2_MSCR_PUS_100K_UP)
#define DDR3_CKEn_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_ODT_60ohm | \
	 SIUL2_MSCR_DDR_INPUT_DIFF_DDR | SIUL2_MSCR_PUS_100K_UP)
#define DDR3_CS_Bn_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_ODT_60ohm | \
	 SIUL2_MSCR_DDR_INPUT_DIFF_DDR | SIUL2_MSCR_PUS_100K_UP)
#define DDR3_BAn_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_DO_TRIM_50PS | \
	 SIUL2_MSCR_DCYCLE_TRIM_LEFT | SIUL2_MSCR_DDR_ODT_60ohm | \
	SIUL2_MSCR_DDR_INPUT_DIFF_DDR |	\
	 SIUL2_MSCR_PUS_100K_UP)
#define DDR3_DMn_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_ODT_60ohm | \
	 SIUL2_MSCR_DDR_INPUT_DIFF_DDR | SIUL2_MSCR_PUS_100K_UP)
#define DDR3_DQSn_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_CRPOINT_TRIM_1 | \
	 SIUL2_MSCR_DCYCLE_TRIM_NONE | SIUL2_MSCR_DDR_ODT_120ohm | \
	SIUL2_MSCR_DDR_INPUT_DIFF_DDR |	\
	 SIUL2_MSCR_PUS_100K_UP)
#define DDR3_ODTn_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_DO_TRIM_50PS | \
	 SIUL2_MSCR_DCYCLE_TRIM_LEFT | SIUL2_MSCR_DDR_INPUT_DIFF_DDR | \
	SIUL2_MSCR_PUS_100K_UP | \
	 SIUL2_MSCR_DDR_ODT_120ohm)
#define DDR3_An_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_DO_TRIM_50PS | \
	 SIUL2_MSCR_DCYCLE_TRIM_LEFT | SIUL2_MSCR_DDR_ODT_60ohm | \
	SIUL2_MSCR_DDR_INPUT_DIFF_DDR |	\
	 SIUL2_MSCR_PUS_100K_UP)
#define DDR3_Dn_PAD	\
	(SIUL2_MSCR_DDR_SEL_DDR3 | SIUL2_MSCR_DSE_34ohm | \
	SIUL2_MSCR_DDR_DO_TRIM_50PS | \
	 SIUL2_MSCR_DCYCLE_TRIM_LEFT | SIUL2_MSCR_DDR_ODT_60ohm | \
	SIUL2_MSCR_DDR_INPUT_DIFF_DDR |	\
	 SIUL2_MSCR_PUS_100K_UP)

/* values of MMDC registers */

#define _MDCTL 0x04190000

/* Set MDSCR[CON_REQ] (configuration request) */
#define MMDC_MDSCR_CFG_VALUE	0x00008000
/* tRFC=139 (260ns),tXS=144 (tRFC+10ns),tXP=4 (6ns),tXPDLL=13 (24ns),
 * tFAW=19(35ns),tCL=7
 */
#define MMDC_MDCFG0_VALUE	0x8A8F7924
/* tRCD=8 (13.75ns),tRP=6 (13,75ns),tRC=26 (48.75ns),tRAS=19 (35ns),
 * tRPA=1,tWR=8 (15ns),tMRD=4,tCWL=6
 */
#define MMDC_MDCFG1_VALUE	0xFF328E64
/* tDLLK=512,tRTP=4,tWTR=4,tRRD=4 */
#define MMDC_MDCFG2_VALUE	0x01FF00DB
/* tAOFPD=3,tAONPD=3,tANPD=5 (tCWL-1),tAXPD=5 (tCWL-1),tODTLon=4 (WL-2),
 * tODT_idle_off=7
 */
#define MMDC_MDOTC_VALUE	0x09444070
/* WALAT=0, BI bank interleave on, MIF3=3, RALAT=1, 8 banks, DDR3 */
#define MMDC_MDMISC_VALUE	0x00001640
/* tXPR=144 (tRFC+10ns), SDE_to_RST=14, RST_to_CKE=33 */
#define MMDC_MDOR_VALUE		0x008F1023

/* P_form ZQ calibration */
#define MMDC_MPZQHWCTRL_VALUE	0xA1390003	/* Force h/w calibration */

/* Complete the initialization sequence as defined by JEDEC */
/* Configure MR2: CWL=6, self-refresh=off, self-refresh temp=normal */
#define MMDC_MDSCR_MR2_VALUE	0x00088032
/* Configure MR3: normal operation */
#define MMDC_MDSCR_MR3_VALUE	0x00008033
/* Configure MR1: enable DLL, drive strength=34R, AL off, ODT=60R,
 * write levelling off, TDQS=0, Qoff=on
 */
#define MMDC_MDSCR_MR1_VALUE	0x00068031
/* Configure MR0: BL=8, CL=7, DLL reset, write recovery=4, precharge PD off */
#define MMDC_MDSCR_MR0_VALUE	0x09308030
#define MMDC_MDSCR_ZQ_VALUE	0x04008040  /* DDR ZQ calibration */

/* Set the amount of DRAM */
/* Set DQS settings based on board type */
#if defined(CONFIG_S32V234EVB_29288) || defined(CONFIG_S32V234BBMINI_29406)
/* 1 GB memory */
#define MMDC_MDASP_MODULE0_VALUE	0x0000007F
/* Read delay line offset */
#define MMDC_MPRDDLCTL_MODULE0_VALUE	0x44434442
/* Write delay line offset */
#define MMDC_MPWRDLCTL_MODULE0_VALUE	0x42434041
/* Read DQS gating control 0 */
#define MMDC_MPDGCTRL0_MODULE0_VALUE	0x41470138
/* Read DQS gating control 1 */
#define MMDC_MPDGCTRL1_MODULE0_VALUE	0x012E0135

/* 1 GB memory */
#define MMDC_MDASP_MODULE1_VALUE	0x0000007F
/* Read delay line offset */
#define MMDC_MPRDDLCTL_MODULE1_VALUE	0x44434442
/* write delay line offset */
#define MMDC_MPWRDLCTL_MODULE1_VALUE	0x3F3F4242
/* Read DQS gating control 0 */
#define MMDC_MPDGCTRL0_MODULE1_VALUE	0x414B0138
/* Read DQS gating control 1 */
#define MMDC_MPDGCTRL1_MODULE1_VALUE	0x01320142
#else
/* 1 GB memory */
#define MMDC_MDASP_MODULE0_VALUE	0x0000007F
/* Read delay line offset */
#define MMDC_MPRDDLCTL_MODULE0_VALUE	0x46464644
/* Write delay line offset */
#define MMDC_MPWRDLCTL_MODULE0_VALUE	0x3B333837
/* Read DQS gating control 0 */
#define MMDC_MPDGCTRL0_MODULE0_VALUE	0x4177016C
/* Read DQS gating control 1 */
#define MMDC_MPDGCTRL1_MODULE0_VALUE	0x013A0141

/* 1 GB memory */
#define MMDC_MDASP_MODULE1_VALUE	0x0000007F
/* Read delay line offset */
#define MMDC_MPRDDLCTL_MODULE1_VALUE	0x45474645
/* write delay line offset */
#define MMDC_MPWRDLCTL_MODULE1_VALUE	0x3E333836
/* Read DQS gating control 0 */
#define MMDC_MPDGCTRL0_MODULE1_VALUE	0x417A0169
/* Read DQS gating control 1 */
#define MMDC_MPDGCTRL1_MODULE1_VALUE	0x0137013F
#endif

/* Read/write command delay - default */
#define MMDC_MDRWD_VALUE		0x000026D2
/* Power down control */
#define MMDC_MDPDC_VALUE		0x00020036
/* Refresh control, 1 refresh each 3.9us */
#define MMDC_MDREF_VALUE		0x08208000
/* 60R nominal */
#define MMDC_MPODTCTRL_VALUE		0x0002222F
/* Deassert the configuration request */
#define MMDC_MDSCR_RESET_VALUE		0x00000000
/* DVFS and LPMD request */
#define MMDC_MAPSR_EN_SLF_REF		0x00300000

/* set I/O pads for DDR */
void ddr_config_iomux(uint8_t module);
void config_mmdc(uint8_t module);

#endif
