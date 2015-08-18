#include <mqx.h>
#include <bsp.h>


static void DDR_power_on(void);
static void DDR_power_off(void);
#define DDR_PWR_PIN			(GPIO_PORT_E|GPIO_PIN8)
#define DDR_PWR_PIN_MUX		(LWGPIO_MUX_E8_GPIO)

LWGPIO_STRUCT DDR_PWR;

/*-----------------------------------------------------------------------------
 *  Function:     power_rail_init
 *  Brief:        Configuring the 5V power rail enable pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void DDR_power_pin_init(void)
{

	if (!lwgpio_init(&DDR_PWR, DDR_PWR_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");
	}

	lwgpio_set_functionality(&DDR_PWR, DDR_PWR_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&DDR_PWR, LWGPIO_VALUE_LOW);

}

/*-----------------------------------------------------------------------------
 *  Function:     DDR_power_on
 *  Brief:        DDR power on.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void DDR_power_on(void)
{

	lwgpio_set_value(&DDR_PWR, LWGPIO_VALUE_HIGH);

}

/*-----------------------------------------------------------------------------
 *  Function:     DDR_power_off
 *  Brief:        Power off DDR. 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void DDR_power_off(void)
{

	lwgpio_set_value(&DDR_PWR, LWGPIO_VALUE_LOW);

}

/*FUNCTION*---------------------------------------------------------------------
 *
 * Function Name    : ddr_init
 * Returned Value   :
 * Comments         :
 *    SDRAM initailization routine.
 *
 *END*-------------------------------------------------------------------------*/
void ddr_init(void)
{

	/* Initialise the DDR Power pin */
	DDR_power_pin_init();

	/* Enables the DDR Power pin */
	DDR_power_on();

    LWGPIO_STRUCT DDR_PWR1;

	if (!lwgpio_init(&DDR_PWR1, (GPIO_PORT_E|GPIO_PIN8), LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");
	}

	lwgpio_set_functionality(&DDR_PWR1, (LWGPIO_MUX_E8_GPIO)); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&DDR_PWR1, LWGPIO_VALUE_HIGH);
	

	if ((SIM_SCGC3 & SIM_SCGC3_DDR_MASK) != SIM_SCGC3_DDR_MASK)
	{
		// Enable DDR clock gate
		SIM_SCGC3 |= SIM_SCGC3_DDR_MASK;
	}
	else
	{
		// Check to if the DRAMC is already initialized
		//		if ((DDR_CR00 & 1) == 1)
		//			return 1;
	}

	// Setup drive strength & enable IO pad
	SIM_MCR &= 0xFFFFFF00;
	SIM_MCR |= 0x00000004;

	// Setup IO pad ODT, 700mV seperation & spare dly ctrl
//#define DDR_PAD_CTRL	*((volatile unsigned long*)0x400AE1AC)
	
	*( (volatile unsigned long *)0x400AE1AC) = 0x00030203;
//	DDR_PAD_CTRL = 0x00030203;

	SIM_MCR |= 0x100;	// DDR RCR reset
	SIM_MCR &= ~0x100;	// DDR RCR out of reset

	DDR_CR00 = 0x00000100;	// DDRCLS, start
	DDR_CR02 = 0x00007530;	// initaref, tinit
	DDR_CR03 = 0x01010707;	// tccd, wrlat, latgate, latlin
	DDR_CR04 = 0x07090201;	// trasmin, trc, trrd, tbint
	DDR_CR05 = 0x02000302;	// tmrd, trtp, trp, twtr
	DDR_CR06 = 0x01290404;	// intwbr, trasmax, tmod
	DDR_CR07 = 0x01010202;	// ccapen, ap, tckesr, clkpw
	DDR_CR08 = 0x06030300;	// tdal, twr, trasdi, tras 
	DDR_CR09 = 0x020000c8;	// bstlen, nocmd, tdll 
	DDR_CR10 = 0x03000002;	// trpab, tcpd, tfaw
	DDR_CR11 = 0x01000000;	// trefen, arefmode, aref, regdimm
	DDR_CR12 = 0x048a000b;	// tref, trfc
	DDR_CR13 = 0x00000005;	// pd, trefint
	DDR_CR14 = 0x00110002;	// txsr, tpdex
	DDR_CR15 = 0x0000000b;	// puref, sref, txsnr
	DDR_CR16 = 0x00000001;	// lpctrl, clkdly, qkref
	DDR_CR20 = 0x00030300;	// cksrx, cksre, lpre
	DDR_CR21 = 0x00000032;	// mr1dat0, mr0dat0
	DDR_CR25 = 0x0a010300;	// aprebit, colsiz, addpins, bnk8
	DDR_CR26 = 0x0101ffff;	// bnkspt, addcol, cmdage, agecnt
	DDR_CR27 = 0x00010101;	// swpen, rwen, prien, plen
	DDR_CR28 = 0x00000001;	// cmdlatr, bigend, reduc, csmap
	DDR_CR30 = 0x00000001;	// intack, rsyncrf
	DDR_CR34 = 0x00000101;	// odtwrcs, odtrdcs
	DDR_CR37 = 0x00000200;	// w2wsame, w2rsame, r2wsame, r2rsame
	DDR_CR38 = 0x00200000;	// p0wrcnt, pupcs, pdncs
	DDR_CR39 = 0x00000020;	// wp0, rp0, p0rdcnt
	DDR_CR40 = 0x00002000;	// p1wrcnt, p0typ
	DDR_CR41 = 0x01010020;	// wp1, rp1, p1rdcnt
	DDR_CR42 = 0x00002000;	// p2wrcnt, p1typ
	DDR_CR43 = 0x02020020;	// wp2, rp2, p2rdcnt
	DDR_CR45 = 0x00070b0f;	// p0pri3, p0pri2, p0pri1, p0pri0
	DDR_CR46 = 0x0f004000;	// p1pri0, p0prirlx, p0ord
	DDR_CR47 = 0x0100070b;	// p1ord, p1pri3, p1pri2, p1pri1
	DDR_CR48 = 0x0b0f0040;	// p2pri1, p2pri0, p1prirlx
	DDR_CR49 = 0x00020007;	// p2ord, p2pri3, p2pri2
	DDR_CR50 = 0x00000040;	// p2prirlx
	DDR_CR52 = 0x02000602;	// rddtenbas, phyrdlat, phywrltbs
	DDR_CR56 = 0x01010000;	// wrlatadj, rdlatadj
	DDR_CR57 = 0x01000000;	// odtalten

	DDR_CR00 |= 1;	// DDRCLS, start

	while ((DDR_CR30 & 0x400) != 0x400)
	{
		
	}

	// Setup memory address translation
	MCM_CR |= MCM_CR_DDRSIZE(0);
}




/*FUNCTION*---------------------------------------------------------------------
 *
 * Function Name    : ddr_disable
 * Returned Value   :
 * Comments         : DDRAM Disabling routine.
 *
 *END*-------------------------------------------------------------------------*/

void ddr_disable(void)
{

	SIM_MemMapPtr   sim = SIM_BASE_PTR;
	//DDR_MemMapPtr   ddr = DDR_BASE_PTR;

	//	ddr->RCR |= DDR_RCR_RST_MASK;

	/* Enable DDR controller clock */
	sim->SCGC3 &= ~SIM_SCGC3_DDR_MASK;

	/* Enable DDR pads and set slew rate */
	sim->MCR   &= ~0xC4;   /* bits were left out of the manual so there isn't a macro right now */

	/* Disables DDR power  */
//	DDR_power_off();
}


