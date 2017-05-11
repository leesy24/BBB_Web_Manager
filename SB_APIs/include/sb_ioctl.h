//=========================================< Serial IOCTL >
#define TIOCMBIC			0x5417
#define TIOCMBIS			0x5416
#define TIOCM_DTR			0x002
#define TIOCM_RTS			0x004
#define TIOCM_LOOP			0x008		/* Internal loopback          */
#define TIOTSVER			0x5460		/* Get Device version         */
#define TIOTSPORTNO			0x5461		/* Get Number of port         */ 
#define TIOTSMSR			0x5462		/* Read Modem Status Register */
#define TIOTSBIS			0x5463		/* Set Break Interrupt        */
#define TIOTSBIC			0x5464		/* Clear Break Interrupt      */
#define TIOTSERROR			0x5466		/* Get error                  */
#define TIOTSEVEN	   		0x5466		/* Set Even Parity			*/
#define TIOTSODD			0x5467		/* Set Odd Parity 		*/
#define TIOTSNOPAR	   		0x5468		/* No Parity   */
#define TIOTGTXCNT			0x5469		/* Get Number of chars in Tx buffer*/
#define TIOTSTRIG			0x5472    /* get or set trigger level -shlee */
#define TIOTSFIFOSIZE		0x5473    /* get or set fifo size -shlee */
#define TIOTSLSR			0x6001		/* Get LSR */
//#define TIOTSPTPNOECHO	0x6004		/* Set Point to Point mode (RS422) or Non Echo mode (RS485) */
//#define TIOTSMULTIECHO	0x6005		/* Set Multi Drop Mode (RS422) or Echo mode (RS485) */
//#define TIOTGSLOTINFO		0x6006		/* Get Slot Information */
#define TIOTSRXCOUNT		0x600d		/* Get Slot Information */

//========================================< GPIO ioctl >
#define	INIT_PRODUCT		0x5500		// init and set product
#define	GET_PRODUCTID		0x5501		// get product ID
#define	RDY_LED_ON			0x5506		// Ready ON
#define	RDY_LED_OFF			0x5507		// Ready OFF
#define	RESET_READ			0x5508		// Read Reset switch 

#define SETGPIOINIT			0x5541
#define SETGPIOMOD_LM		0x5517	// low multi control 
#define GETGPIOMOD_LM		0x5518
#define SETGPIOVAL_LM		0x5519
#define GETGPIOVAL_LM		0x5520
#define SETGPIOPUL_LM		0x5521
#define GETGPIOPUL_LM		0x5522	
#define SETGPIOMOD_LA		0x5523	// low Port A.
#define GETGPIOMOD_LA		0x5524
#define SETGPIOVAL_LA		0x5525
#define GETGPIOVAL_LA		0x5526
#define SETGPIOPUL_LA		0x5527
#define GETGPIOPUL_LA		0x5528
#define SETGPIOMOD_LB		0x5529	// low Port B.
#define GETGPIOMOD_LB		0x5530
#define SETGPIOVAL_LB		0x5531
#define GETGPIOVAL_LB		0x5532
#define SETGPIOPUL_LB		0x5533
#define GETGPIOPUL_LB		0x5534
#define SETGPIOMOD_LC		0x5535	// low Port C.
#define GETGPIOMOD_LC		0x5536
#define SETGPIOVAL_LC		0x5537
#define GETGPIOVAL_LC		0x5538
#define SETGPIOPUL_LC		0x5539
#define GETGPIOPUL_LC		0x5540


#define	INTERFACESEL		0x5504		// Eddy Serial interface select (422,485)


/*
#define	COMRXON				0x5502		// RS485  RX ON, TX Off
#define	COMTXON				0x5503		// RS485  RX Off, TX ON
#define	INTERFACESEL		0x5504		// Eddy Serial interface select (422,485)
#define	HW_RESET			0x5509		// HW Reset
#define SETGPIOMODEIN       0x5510 	// set gpio mode input 
#define SETGPIOMODEOUT      0x5511 	// set gpio mode output
#define GETGPIOMODE         0x5512 	// get gpio mode input = 1 output = 0 
#define SETGPIOVALUEHIGH    0x5513 	// set gpio value high 
#define SETGPIOVALUELOW     0x5514 	// set gpio value low 
#define GETGPIOVALUE        0x5515 	// get gpio value high = 1 low = 0 
#define SETGPIOMODE_MULTI   0x5517 	// set gpio mode Multi
#define GETGPIOMODE_MULTI	0x5518 	// get gpio mode Multi input = 1 output = 0 
#define SETGPIOPULLUP_MULTI 0x5519  //
#define GETGPIOPULLUP_MULTI 0x5520  //
#define SETGPIOVALUE_MULTI  0x5521  //
#define GETGPIOVALUE_MULTI  0x5522  //
*/
//========================================< ADC ioctl >
#define ADCGETVALUE         0x5600  /* get digital value */
#define ADCGETMODE          0x5601  /* get mode register setting*/
#define ADCSETMODE          0x5602  /* set mode register setting*/
#define ADCGETCHANNEL       0x5603  /* get enabled channel info*/
#define ADCSETCHANNEL       0x5604  /* enable the channel */


