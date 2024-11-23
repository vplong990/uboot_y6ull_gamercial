// SPDX-License-Identifier:     GPL-2.0+
/*
 * Copyright 2015-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2017,2020 NXP
 *
 */

#include <common.h>
#include <vsprintf.h>
#include <cpu_func.h>
#include <asm/arch/qspi_common.h>
#include <asm/arch/siul.h>
#include <asm/io.h>

#define	SEQID_LUT			15

#define QUADSPI_MCR			0x00
#define QUADSPI_MCR_RESERVED_MASK	GENMASK(19, 16)
#define QUADSPI_MCR_MDIS		BIT(14)
#define QUADSPI_MCR_CLR_TXF		BIT(11)
#define QUADSPI_MCR_CLR_RXF		BIT(10)
#define QUADSPI_MCR_DDR_EN		BIT(7)
#define QUADSPI_MCR_DQS_EN		BIT(6)
#define QUADSPI_MCR_DQS_LAT_EN		BIT(5)
#define QUADSPI_MCR_END_CFG_MASK	GENMASK(3, 2)
#define QUADSPI_MCR_SWRSTHD		BIT(1)
#define QUADSPI_MCR_SWRSTSD		BIT(0)

#define QUADSPI_BUF3CR			0x1c

#define QUADSPI_IPCR			0x08
#define QUADSPI_IPCR_SEQID(x)		((x) << 24)

#define QUADSPI_FLSHCR			0x0c
#define QUADSPI_FLSHCR_TDH_VALUE	0x1
#define QUADSPI_FLSHCR_TDH_MASK		GENMASK(17, 16)
#define QUADSPI_FLSHCR_OFFSET		16
#define QUADSPI_FLSHCR_TDH(x)		(((x) << QUADSPI_FLSHCR_OFFSET) & \
					QUADSPI_FLSHCR_TDH_MASK)

#define QUADSPI_BFGENCR			0x20

#define QUADSPI_BUF0IND			0x30
#define QUADSPI_BUF1IND			0x34
#define QUADSPI_BUF2IND			0x38
#define QUADSPI_SFAR			0x100

#define QUADSPI_SFACR			0x104
#define QUADSPI_SFACR_WA		BIT(16)
#define QUADSPI_SFACR_CAS_VALUE		0x3
#define QUADSPI_SFACR_CAS_MASK		GENMASK(3, 0)
#define QUADSPI_SFACR_CAS_OFFSET	0
#define QUADSPI_SFACR_CAS(x)		(((x) << QUADSPI_SFACR_CAS_OFFSET) & \
					QUADSPI_SFACR_CAS_MASK)

#define QUADSPI_RBSR			0x10C
#define QUADSPI_RBSR_RDBFL_MASK		GENMASK(13, 8)
#define QUADSPI_RBSR_RDBFL_OFFSET	0x8
#define QUADSPI_RBSR_RDBFL_VALUE(x)	(((x) & QUADSPI_RBSR_RDBFL_MASK) >> \
					QUADSPI_RBSR_RDBFL_OFFSET)

#define QUADSPI_TBDR			0x154

#define QUADSPI_SR			0x15C
#define QUADSPI_SR_IP_ACC		BIT(1)
#define QUADSPI_SR_AHB_ACC		BIT(2)

#define QUADSPI_FR			0x160
#define QUADSPI_FR_RBDF			BIT(16)
#define QUADSPI_FR_TBFF			BIT(27)

#define QUADSPI_SFA1AD			0x180
#define QUADSPI_SFA2AD			0x184
#define QUADSPI_SFB1AD			0x188
#define QUADSPI_SFB2AD			0x18c
#define QUADSPI_RBDR(x)			(0x200 + ((x) * 4))

#define QUADSPI_LUTKEY			0x300
#define QUADSPI_LUTKEY_VALUE		0x5AF05AF0

#define QUADSPI_LCKCR			0x304
#define QUADSPI_LCKER_LOCK		BIT(0)
#define QUADSPI_LCKER_UNLOCK		BIT(1)

#define QUADSPI_LUT_BASE		0x310
#define QUADSPI_LUT_REG(idx)		(QUADSPI_LUT_BASE + (idx) * 4)

/* QUADSPI Instructions */
#define CMD		1
#define ADDR		2
#define DUMMY		3
#define MODE		4
#define MODE2		5
#define MODE4		6
#define READ		7
#define WRITE		8
#define JMP_ON_CS	9
#define ADDR_DDR	10
#define MODE_DDR	11
#define MODE2_DDR	12
#define MODE4_DDR	13
#define READ_DDR	14
#define WRITE_DDR	15
#define DATA_LEARN	16
#define CMD_DDR		17
#define CADDR		18
#define CADDR_DDR	19
#define STOP		0

#define QSPI_LUT(CMD1, PAD1, OP1, CMD0, PAD0, OP0)	\
	((((CMD1) & 0x3f) << 26) | \
	 (((PAD1) & 3) << 24) | \
	 (((OP1) & 0xff) << 16) | \
	 (((CMD0) & 0x3f) << 10) | \
	 (((PAD0) & 3) << 8) | ((OP0) & 0xff))

#define QSPI_BASE	QSPI_BASE_ADDR
#define BURST_SIZE	512
#define FLASH_HALF_PAGE_SIZE	16

#define QSPI_LUT_0		0
#define QSPI_LUT_1		1
#define QSPI_LUT_2		2
#define QSPI_LUT_60		60
#define QSPI_LUT_61		61
#define QSPI_LUT_62		62

enum qspi_addr_t {
	qspi_real_address = 1,
	qspi_real_and_all
};

static void qspi_writel(unsigned int val, unsigned long addr)
{
	out_le32(addr + QSPI_BASE_ADDR, val);
}

static u32 qspi_readl(unsigned long addr)
{
	return in_le32(addr + QSPI_BASE_ADDR);
}

static void quadspi_set_lut(u32 index, u32 value)
{
	/* unlock LUT */
	qspi_writel(QUADSPI_LUTKEY_VALUE, QUADSPI_LUTKEY);
	qspi_writel(QUADSPI_LCKER_UNLOCK, QUADSPI_LCKCR);

	qspi_writel(value, QUADSPI_LUT_REG(index));

	/* lock LUT */
	qspi_writel(QUADSPI_LUTKEY_VALUE, QUADSPI_LUTKEY);
	qspi_writel(QUADSPI_LCKER_LOCK, QUADSPI_LCKCR);
}

static void quadspi_read_hyp(void)
{
	quadspi_set_lut(QSPI_LUT_0,
			QSPI_LUT(ADDR_DDR, 3, 24, CMD_DDR, 3, 0xA0));
	quadspi_set_lut(QSPI_LUT_1,
			QSPI_LUT(DUMMY, 3, 15, CADDR_DDR, 3, 16));
	quadspi_set_lut(QSPI_LUT_2,
			QSPI_LUT(STOP, 3, 0, READ_DDR, 3, 128));
	qspi_writel(0, QUADSPI_BFGENCR);
}

/*
 * If we have changed the content of the flash by writing or erasing,
 * we need to invalidate the AHB buffer. If we do not do so, we may read out
 * the wrong data. The spec tells us reset the AHB domain and Serial Flash
 * domain at the same time.
 */
static inline void qspi_ahb_invalid(void)
{
	u32 reg;

	reg = qspi_readl(QUADSPI_MCR);
	reg |= QUADSPI_MCR_SWRSTHD | QUADSPI_MCR_SWRSTSD;
	qspi_writel(reg, QUADSPI_MCR);

	/*
	 * The minimum delay : 1 AHB + 2 SFCK clocks.
	 * Delay 1 us is enough.
	 */
	udelay(1);

	reg &= ~(QUADSPI_MCR_SWRSTHD | QUADSPI_MCR_SWRSTSD);
	qspi_writel(reg, QUADSPI_MCR);
}

static void qspi_setup_hyp(void)
{
	qspi_iomux();

#ifdef CONFIG_DEBUG_S32V234_QSPI_QSPI
	eprintf("ERROR: %s HAS NO QuadSPI settings and definitions", __func__);
#else
	debug("%s uses baremetal QuadSPI settings and definitions", __func__);

	qspi_writel(qspi_readl(QUADSPI_MCR) &
		    ~QUADSPI_MCR_MDIS, QUADSPI_MCR);
	/* set AHB buffer size (64bits) */
	qspi_writel(0, QUADSPI_BUF0IND);
	/* set top address of FA1 (size 512Mbit) */
	qspi_writel(CONFIG_SYS_FLASH_BASE + 0x4000000, QUADSPI_SFA1AD);
	/* set top address of FA2 (size 0Mbit) */
	qspi_writel(CONFIG_SYS_FLASH_BASE + 0x4000000, QUADSPI_SFA2AD);
	/* set top address of FB1 (size 512Mbit) */
	qspi_writel(FLASH_BASE_ADR2 + 0x4000000, QUADSPI_SFB1AD);
	/* set top address of FB2 (size 0Mbit) 0x203FFFFF */
	qspi_writel(FLASH_BASE_ADR2 + 0x4000000, QUADSPI_SFB2AD);

	qspi_writel(0x100, QUADSPI_BUF0IND);	/* buffer0 size 512 bytes */
	qspi_writel(0x200, QUADSPI_BUF1IND);	/* buffer1 size 0 bytes */
	qspi_writel(0x200, QUADSPI_BUF2IND);	/* buffer2 size 0 bytes */
	qspi_writel(0x80000000, QUADSPI_BUF3CR);/* All masters use buffer 3 */

	qspi_writel(CONFIG_SYS_FLASH_BASE, QUADSPI_SFAR);
	qspi_writel(qspi_readl(QUADSPI_MCR) | QUADSPI_MCR_DQS_LAT_EN |
		    QUADSPI_MCR_DDR_EN | QUADSPI_MCR_DQS_EN,
		    QUADSPI_MCR);
	qspi_writel(QUADSPI_SFACR_WA |
		    QUADSPI_SFACR_CAS(QUADSPI_SFACR_CAS_VALUE),
		    QUADSPI_SFACR);
	qspi_writel((qspi_readl(QUADSPI_FLSHCR) & ~QUADSPI_FLSHCR_TDH_MASK) |
		    QUADSPI_FLSHCR_TDH(QUADSPI_FLSHCR_TDH_VALUE),
		    QUADSPI_FLSHCR);

	/* Set-up Read command for hyperflash. The command will be used
	 * by default for any AHB access to the flash memory.
	 */
	quadspi_read_hyp();
#endif
}

static void quadspi_send_instruction_hyp(unsigned int address, unsigned int cmd)
{
	qspi_writel(address & 0xFFFFFFFE, QUADSPI_SFAR);
	quadspi_set_lut(QSPI_LUT_60,
			QSPI_LUT(ADDR_DDR, 3, 0x18, CMD_DDR, 3, 0x00));
	quadspi_set_lut(QSPI_LUT_61,
			QSPI_LUT(CMD_DDR, 3, cmd >> 8, CADDR_DDR, 3, 0x10));
	quadspi_set_lut(QSPI_LUT_62,
			QSPI_LUT(STOP, 0, 0, CMD_DDR, 3, cmd));
	qspi_writel(QUADSPI_IPCR_SEQID(SEQID_LUT), QUADSPI_IPCR);
	while (qspi_readl(QUADSPI_SR) & QUADSPI_SR_IP_ACC)
		;
}

static unsigned int quadspi_status_hyp(void)
{
	unsigned int data;

	quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0xAAA, 0x70);

	qspi_writel(CONFIG_SYS_FLASH_BASE + 0x2, QUADSPI_SFAR);
	quadspi_set_lut(QSPI_LUT_60, QSPI_LUT(ADDR_DDR, 3, 0x18, CMD_DDR, 3, 0x80));
	quadspi_set_lut(QSPI_LUT_61, QSPI_LUT(DUMMY, 3, 15, CADDR_DDR, 3, 0x10));
	quadspi_set_lut(QSPI_LUT_62, QSPI_LUT(STOP, 0, 0, READ_DDR, 3, 0x2));

	qspi_writel(qspi_readl(QUADSPI_MCR) |
		    QUADSPI_MCR_CLR_RXF, QUADSPI_MCR);
	qspi_writel(QUADSPI_FR_RBDF, QUADSPI_FR);
	/* fill the RX buffer */
	qspi_writel(2 | QUADSPI_IPCR_SEQID(SEQID_LUT), QUADSPI_IPCR);
	while (qspi_readl(QUADSPI_SR) & QUADSPI_SR_IP_ACC)
		;
	while (QUADSPI_RBSR_RDBFL_VALUE(qspi_readl(QUADSPI_RBSR)) != 1)
		;

	data = qspi_readl(QUADSPI_RBDR(0));
	return data;
}

/* address = -1 means chip erase */
static void quadspi_erase_hyp(int address)
{
	int address_start = address & ~(FLASH_SECTOR_SIZE - 1);

	/* Invalidate all cache data. */
	qspi_ahb_invalid();
	invalidate_icache_all();
	if (address == -1)
		invalidate_dcache_range(CONFIG_SYS_FLASH_BASE,
					CONFIG_SYS_FLASH_BASE +
					CONFIG_SYS_FSL_FLASH0_SIZE);
	else
		invalidate_dcache_range(address_start,
					address_start + FLASH_SECTOR_SIZE);

	//check status, wait to be ready
	while ((quadspi_status_hyp() & 0x8000) == 0)
		;

	quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0xAAA, 0xAA);
	quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0x554, 0x55);

	quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0xAAA, 0x80);
	quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0xAAA, 0xAA);

	quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0x554, 0x55);

	if (address == -1)
		quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0xAAA,
					     0x10);
	else
		quadspi_send_instruction_hyp((address & 0xfffffffe), 0x30);

	//check status, wait to be ready
	while ((quadspi_status_hyp() & 0x8000) == 0)
		;
}

static void quadspi_program_hyp(unsigned int address, uintptr_t pdata,
				unsigned int bytes)
{
	/* i: number total of bytes to flash
	 * k: number of bytes to flash at the next command
	 * m :number of dword (32 bits) to flash at the next command
	 */
	int i, j, k, m;
	unsigned int *data;

	data = (unsigned int *)pdata;
	i = bytes;

	/* Invalidate all cache data. */
	qspi_ahb_invalid();
	invalidate_icache_all();
	invalidate_dcache_range(address, address + i);

	//check status, wait to be ready
	while ((quadspi_status_hyp() & 0x8000) == 0)
		;

	while (i > 0) {
		/* 128 is the circular TX Buffer depth. */
		k = i >= 128 ? 128 : i;

		/* 512-byte address boundary should not be crossed so write
		 * the minimum number between k and the left number of bytes
		 * in the current burst area.
		 */
		k = min(k, (int)(BURST_SIZE - address % BURST_SIZE));

		/* Compute the number of dwords. */
		m = k >> 2;

		quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0xAAA,
					     0xAA);
		quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0x554,
					     0x55);
		quadspi_send_instruction_hyp(CONFIG_SYS_FLASH_BASE + 0xAAA,
					     0xA0);

#ifndef CONFIG_DEBUG_S32V234_QSPI
		debug("%s uses baremetal QuadSPI settings and definitions",
		      __func__);
#endif
		/* prepare write/program instruction */
		qspi_writel(address, QUADSPI_SFAR);
		quadspi_set_lut(QSPI_LUT_60,
				QSPI_LUT(ADDR_DDR, 3, 0x18, CMD_DDR, 3, 0x00));
		quadspi_set_lut(QSPI_LUT_61,
				QSPI_LUT(WRITE_DDR, 3, 2, CADDR_DDR, 3, 0x10));
		quadspi_set_lut(QSPI_LUT_62, 0);
		/* tx buffer */
		qspi_writel(qspi_readl(QUADSPI_MCR) |
			    QUADSPI_MCR_CLR_TXF, QUADSPI_MCR);
		qspi_writel(QUADSPI_FR_TBFF, QUADSPI_FR);
		/* load write data */
		for (j = 0; j < m; j++)
			qspi_writel(*data++, QUADSPI_TBDR);
		qspi_writel(k | QUADSPI_IPCR_SEQID(SEQID_LUT), QUADSPI_IPCR);
		/* wait for cmd to be sent */
		while (qspi_readl(QUADSPI_SR) & QUADSPI_SR_IP_ACC)
			;
		/* check status, wait to be done */
		while ((quadspi_status_hyp() & 0x8000) == 0)
			;

		address += k;
		i -= k;
	}

	/* check status, wait to be ready */
	while ((quadspi_status_hyp() & 0x8000) == 0)
		;
}

int do_qspinor_setup(cmd_tbl_t *cmdtp, int flag, int argc,
		     char * const argv[])
{
	printf("SD/eMMC is disabled. Hyperflash is active and can be used!\n");
	qspi_setup_hyp();
	return 0;
}

static bool is_flash_addr(unsigned int address, enum qspi_addr_t addr_type)
{
	bool isflash = 0;

	isflash |= (address >= CONFIG_SYS_FLASH_BASE &&
		    address < CONFIG_SYS_FLASH_BASE + CONFIG_SYS_FSL_FLASH0_SIZE);
	isflash |= (addr_type == qspi_real_and_all) && (address == -1);
	if (!isflash) {
		printf("Incorrect address '0x%.8x'.\n"
		       "Must an address above or equal to '0x%.8x' (or '-1',"
		       " if the command accepts it)\n", address,
		       CONFIG_SYS_FLASH_BASE);
		return 0;
	}
	return 1;
}

volatile static bool flash_lock = 1;
static int do_qspinor_prog(cmd_tbl_t *cmdtp, int flag, int argc,
			   char * const argv[])
{
	unsigned int fladdr, bufaddr, size;

	if (argc != 4) {
		printf("This command needs exactly three parameters (flashaddr "
				"buffaddr and size).\n");
		return 1;
	}

	fladdr = simple_strtol(argv[1], NULL, 16);
	if (!is_flash_addr(fladdr, qspi_real_address))
		return 1;

	if (fladdr % FLASH_HALF_PAGE_SIZE != 0)
		printf("Address should be %d bytes aligned.\n",
		       FLASH_HALF_PAGE_SIZE);

	bufaddr = simple_strtol(argv[2], NULL, 16);
	size = simple_strtol(argv[3], NULL, 16);

	/* It is strongly recommended that a multiple of 16-byte half-pages be
	 * written and each half-page written only once.
	 */
	if (size < FLASH_HALF_PAGE_SIZE || size % FLASH_HALF_PAGE_SIZE != 0) {
		printf("The written size must be multiple of %d.\n",
		       FLASH_HALF_PAGE_SIZE);
		return 1;
	}

	if (!flash_lock)
		quadspi_program_hyp(fladdr, (uintptr_t)bufaddr, size);
	else
		printf("Flash write and erase operations are locked!\n");

	return 0;
}

static int do_qspinor_erase(cmd_tbl_t *cmdtp, int flag, int argc,
			    char * const argv[])
{
	long addr_start;

	if (argc != 2) {
		printf("This command needs exactly one parameter\n");
		return 1;
	}

	addr_start = simple_strtol(argv[1], NULL, 16);
	if (!is_flash_addr(addr_start, qspi_real_and_all))
		return 1;

	if (!flash_lock)
		quadspi_erase_hyp(addr_start);
	else
		printf("Flash write and erase operations are locked!\n");
	return 0;
}

/* we only need our own SW protect until we implement proper protection via HW
 * mechanisms; until then we need not conflict with those commands
 */
#ifndef CONFIG_CMD_FLASH
/* we clean (set to 0) the LUTs used for write and erase to make sure no
 * accidental writes or erases can happen
 */
void quadspi_rm_write_erase_luts(void)
{
	quadspi_set_lut(QSPI_LUT_60, 0);
	quadspi_set_lut(QSPI_LUT_61, 0);
	quadspi_set_lut(QSPI_LUT_62, 0);
}

static int do_swprotect(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	if (argc != 2) {
		printf("This command needs exactly one parameter (on/off).\n");
		return 1;
	}

	if (!strcmp(argv[1], "on")) {
		quadspi_rm_write_erase_luts();
		flash_lock = 1;
		return 0;
	}

	if (!strcmp(argv[1], "off")) {
		flash_lock = 0;
		return 0;
	}

	printf("Unexpected parameter. This command accepts only 'on' and 'off' as parameter.\n");
	return 1;
}

/* simple SW protection */
U_BOOT_CMD(protect, 2, 1, do_swprotect,
	   "protect on/off the flash memory against write and erase operations",
	   "on\n"
	   "    - enable protection and forbid erase and write operations\n"
	   "protect off\n"
	   "    - disable protection allowing write and erase operations\n"
	   ""
	  );

/* quadspi_erase_hyp */
U_BOOT_CMD(erase, 3, 1, do_qspinor_erase,
	   "erase FLASH from address 'START'",
	   "erase START / -1\n"
	   "    - erase flash starting from START address\n"
	   "    - if START=-1, erase the entire chip\n"
	  );
#else
#warning "Using U-Boot's protect and erase commands, not our custom ones"
#endif

/* quadspi_erase_hyp */
U_BOOT_CMD(flwrite, 4, 1, do_qspinor_prog,
	   "write a data buffer into hyperflash",
	   "ADDR BUFF HEXLEN\n"
	   "    - write into flash starting with address ADDR\n"
	   "      the first HEXLEN bytes contained in the memory\n"
	   "      buffer at address BUFF.\n"
	   "      Note: all numbers are in hexadecimal format\n"
	  );

#ifdef CONFIG_ENV_IS_IN_FLASH

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

/*
 * Wrappers for our flash memory functions, since
 * common/env_flash.c's saveenv function expects those
 * with different signatures. Done to enable the saving
 * of u-boot's environment to flash memory. Temporary
 * solution until this driver is updated to adhere to
 * the U-boot Driver Model.
 */

int flash_sect_protect(int protect_on, unsigned long flash_begin_addr,
		       unsigned long flash_end_addr)
{
	if (protect_on) {
		quadspi_rm_write_erase_luts();
		flash_lock = 1;
	} else {
		flash_lock = 0;
	}

	return 0;
}

int flash_sect_erase(unsigned long flash_begin_addr,
		     unsigned long flash_end_addr)
{
	if (!is_flash_addr((long)flash_begin_addr, qspi_real_and_all))
		return 1;

	if (!flash_lock)
		quadspi_erase_hyp(flash_begin_addr);
	else
		return -1;

	return 0;
}

unsigned long flash_init(void)
{
	int i;

	flash_info[0].size = CONFIG_SYS_MAX_FLASH_SECT * FLASH_SECTOR_SIZE;
	flash_info[0].sector_count = CONFIG_SYS_MAX_FLASH_SECT;
	flash_info[0].flash_id = 0;

	flash_info[0].start[0] = CONFIG_SYS_FLASH_BASE;
	flash_info[0].protect[0] = 0;

	for (i = 1; i < flash_info[0].sector_count; i++) {
		flash_info[0].start[i] = flash_info[0].start[i - 1] +
				CONFIG_ENV_SECT_SIZE; /* 256 KB */
		flash_info[0].protect[i] = 0;
	}

	return flash_info[0].size;
}

int flash_write(char *data, ulong flash_addr, ulong bytes)
{
	quadspi_program_hyp((unsigned int)flash_addr, (uintptr_t)data,
			    (unsigned int)bytes);
	return 0;
}

/* The flash_perror() function is only called with return codes from
 * flash_write(). Since we have a custom implementation for the later
 * which always returns 0, there is nothing to be done in flash_perror().
 */

void flash_perror(int rc) {}

#endif
