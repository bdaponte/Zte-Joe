


#include "msm_fb.h"
#include <asm/gpio.h>
#include <linux/module.h>
#include <linux/delay.h>

#define GPIO_LCD_BL_SC_OUT 97
#define GPIO_LCD_BL_EN

#define LCD_BL_LEVEL 32
#define lcd_bl_max   lcd_bl_level-1
#define lcd_bl_min   0

static int lcdc_samsung_regist = 1;
extern uint32 gpio_ic_lead,gpio_ic_himax;

static int spi_cs;
static int spi_sclk;
static int spi_sdi;
static int spi_sdo;
static int samsung_reset;


static struct msm_panel_common_pdata * lcdc_samsung_pdata;
static void gpio_lcd_emuspi_write_more_para(int argc, unsigned short argv, ...);
static void lcdc_samsung_sleep(void);
static void lcdc_samsung_init(void);
static void lcdc_set_bl(struct msm_fb_data_type *mfd);

static void spi_init(void);
static int lcdc_panel_on(struct platform_device *pdev);
static int lcdc_panel_off(struct platform_device *pdev);

static int lcdc_panel_on(struct platform_device *pdev)
{

	spi_init();
		lcdc_samsung_init();
		
	return 0;
}

static void lcdc_samsung_sleep(void)
{

	gpio_lcd_emuspi_write_more_para(0,0x29);
	mdelay(10);
	gpio_lcd_emuspi_write_more_para(0,0x10);
	mdelay(120);
}

static void lcdc_samsung_init(void)
{

	gpio_direction_output(samsung_reset, 1);
	mdelay(10);
	gpio_direction_output(samsung_reset, 0);
	mdelay(20);
	gpio_direction_output(samsung_reset, 1);
	mdelay(10);


	gpio_lcd_emuspi_write_more_para(2,0xf0,0x5a,0x5a);

	gpio_lcd_emuspi_write_more_para(1,0xf3,0x00);
	gpio_lcd_emuspi_write_more_para(4,0xff,0x00,0x00,0x00,0x40);
	gpio_lcd_emuspi_write_more_para(0,0x11);
	mdelay(20);
	gpio_lcd_emuspi_write_more_para(8,0xf3,0x01,0x26,0x26,0x0B,0x22,0x66,0x60,0x2c);
	gpio_lcd_emuspi_write_more_para(5,0xf4,0x5D,0x60,0x6d,0x6d,0x44);
	gpio_lcd_emuspi_write_more_para(6,0xf5,0x02,0x11,0x0A,0xf0,0x33,0x1f);
	mdelay(40);
	gpio_lcd_emuspi_write_more_para(1,0xf3,0x03);
	mdelay(20);
	gpio_lcd_emuspi_write_more_para(4,0xff,0x00,0x00,0x00,0x60);
	mdelay(20);
	gpio_lcd_emuspi_write_more_para(1,0xf3,0x07);
	gpio_lcd_emuspi_write_more_para(4,0xff,0x00,0x00,0x00,0x70);
	mdelay(40);
	gpio_lcd_emuspi_write_more_para(1,0xf3,0x0f);
	gpio_lcd_emuspi_write_more_para(4,0xff,0x00,0x00,0x00,0x78);
	mdelay(40);
	gpio_lcd_emuspi_write_more_para(1,0xf3,0x1f);
	mdelay(40);
	gpio_lcd_emuspi_write_more_para(1,0xf3,0x3f);
	mdelay(60);
	gpio_lcd_emuspi_write_more_para(1,0xff,0x00);
	gpio_lcd_emuspi_write_more_para(11,0xf2,0x16,0x16,0x01,0x0f,0x0f,0x0f,0x0f,0x1f,0x00,0x10,0x10);
	gpio_lcd_emuspi_write_more_para(1,0xfd,0x22);
	gpio_lcd_emuspi_write_more_para(1,0x3a,0x66);
	gpio_lcd_emuspi_write_more_para(1,0x36,0x48);
	gpio_lcd_emuspi_write_more_para(4,0xf6,0x00,0x01,0xf0,0x10);

	gpio_lcd_emuspi_write_more_para(15,0xf7,0x8f,0x15,0x2b,0x2c,0x30,0x29,0x26,0x27,0x1e,0x22,0x29,0x23,0x08,0x11,0x11);
	gpio_lcd_emuspi_write_more_para(15,0xf8,0x80,0x00,0x17,0x1a,0x1d,0x1e,0x23,0x2e,0x12,0x15,0x22,0x1e,0x04,0x11,0x11);

	gpio_lcd_emuspi_write_more_para(1,0x51,0x7e);
	gpio_lcd_emuspi_write_more_para(1,0x53,0x2c);
	gpio_lcd_emuspi_write_more_para(1,0x55,0x00);
	gpio_lcd_emuspi_write_more_para(1,0x5e,0x00);
	gpio_lcd_emuspi_write_more_para(3,0xca,0x80,0x80,0x20);
	gpio_lcd_emuspi_write_more_para(1,0xcb,0x01);
	gpio_lcd_emuspi_write_more_para(2,0xcd,0x7f,0x3f);

	gpio_lcd_emuspi_write_more_para(1,0xf3,0x7f);

	gpio_lcd_emuspi_write_more_para(0,0x29);

	pr_debug("lcd module samsung init exit!\n");
	
}


static void lcdc_set_bl(struct msm_fb_data_type *mfd)
{

    int current_lel = mfd->bl_level;
    uint8_t cnt = 0;
    unsigned long flags;


    printk( "[ZYF] lcdc_set_bl level=%d, %d\n", 
		   current_lel , mfd->panel_power_on);


    if(!mfd->panel_power_on)
	{
    	    gpio_direction_output(GPIO_LCD_BL_SC_OUT, 0);		
	    return;
    	}

    if(current_lel < 1)
    {
        current_lel = 0;
    }
    if(current_lel > 32)
    {
        current_lel = 32;
    }

    local_irq_save(flags);
    if(current_lel==0)
	    gpio_direction_output(GPIO_LCD_BL_SC_OUT, 0);
    else {
	    for(cnt = 0;cnt < 33-current_lel;cnt++) 
	    { 
		    gpio_direction_output(GPIO_LCD_BL_SC_OUT, 1);
		    udelay(20);
		    gpio_direction_output(GPIO_LCD_BL_SC_OUT, 0);
		    udelay(5);
	    }     
	    gpio_direction_output(GPIO_LCD_BL_SC_OUT, 1);
	    mdelay(8);

    }
    local_irq_restore(flags);
}

static void spi_init(void)
{
	spi_sclk = *(lcdc_samsung_pdata->gpio_num);
	spi_cs   = *(lcdc_samsung_pdata->gpio_num + 1);
	spi_sdi  = *(lcdc_samsung_pdata->gpio_num + 2);
	spi_sdo  = *(lcdc_samsung_pdata->gpio_num + 3);
	samsung_reset = *(lcdc_samsung_pdata->gpio_num + 4);

	gpio_set_value(spi_sclk, 1);
	gpio_set_value(spi_sdo, 1);
	gpio_set_value(spi_cs, 1);
	mdelay(10);

	gpio_direction_output(samsung_reset, 1);
	mdelay(10);
	gpio_direction_output(samsung_reset, 0);
	mdelay(5);
	gpio_direction_output(samsung_reset, 1);
	mdelay(60);
}
static int lcdc_panel_off(struct platform_device *pdev)
{

	lcdc_samsung_sleep();

	gpio_direction_output(samsung_reset, 0);
	gpio_direction_output(spi_sclk, 0);
	gpio_direction_output(spi_sdi, 0);
	gpio_direction_output(spi_sdo, 0);
	gpio_direction_output(spi_cs, 0);
	mdelay(100);

	return 0;
}



static void gpio_lcd_emuspi_write_more_para(int argc, unsigned short argv, ...)
{
	unsigned short i,addr;
	int j,num=0;
	va_list args;

	va_start(args, argv);
	addr= argv;


	gpio_direction_output(spi_cs, 0);
	i = addr| 0x000;
	
	for (j = 0; j < 9; j++) {
		if (i & 0x100)
			gpio_direction_output(spi_sdo, 1);	
		else
			gpio_direction_output(spi_sdo, 0);	

		gpio_direction_output(spi_sclk, 0);	
		/*udelay(4);*/
		gpio_direction_output(spi_sclk, 1);	
		/*udelay(4);*/
		i <<= 1;
	}
	gpio_direction_output(spi_cs, 1);
	/*udelay(4);*/
	while(num < argc){
		gpio_direction_output(spi_cs, 0);
		/*udelay(4);*/
		
		i= va_arg(args, int );// | 0x7200;
		
		i = i | 0x100;
	
		for (j = 0; j < 9; j++) {

			if (i & 0x100)
				gpio_direction_output(spi_sdo, 1);
			else
				gpio_direction_output(spi_sdo, 0);

			gpio_direction_output(spi_sclk, 0);	
			/*udelay(4);*/
			gpio_direction_output(spi_sclk, 1);
			/*udelay(4);*/
			i <<= 1;
		}
		gpio_direction_output(spi_cs, 1);
		num++;
	}
	va_end(args);
}

static struct msm_fb_panel_data lcdc_samsung_panel_data = {
       .panel_info = {.bl_max = 32},
	.on = lcdc_panel_on,
	.off = lcdc_panel_off,
       .set_backlight = lcdc_set_bl,
};

static struct platform_device this_device = {
	.name   = "lcdc_panel_qvga",
	.id	= 1,
	.dev	= {
		.platform_data = &lcdc_samsung_panel_data,
	}
};

static int __init lcdc_panel_probe(struct platform_device *pdev)
{
	struct msm_panel_info *pinfo;
	uint32 gpio_ic6[6];
	int ret;

	gpio_ic6[0]=1234;
	if(pdev->id == 0) {
		lcdc_samsung_pdata = pdev->dev.platform_data;
		lcdc_samsung_pdata->panel_config_gpio(1);
		spi_init();	
	 if((gpio_ic_himax == 0x47) || (gpio_ic_lead == 0x9325)){
			printk("Fail to register this samsung driver!\n");
			lcdc_samsung_regist = 0;
			return -ENODEV;
	 }
	 else{
		pinfo = &lcdc_samsung_panel_data.panel_info;
		pinfo->xres = 240;
		pinfo->yres = 320;
		pinfo->type = LCDC_PANEL;
		pinfo->pdest = DISPLAY_1;
		pinfo->wait_cycle = 0;
		pinfo->bpp = 18;
		pinfo->fb_num = 2;

		pinfo->clk_rate = 6144000;
		
		pinfo->lcdc.h_back_porch = 18;
		pinfo->lcdc.h_front_porch = 3;
		pinfo->lcdc.h_pulse_width = 4;
		pinfo->lcdc.v_back_porch = 8;
		pinfo->lcdc.v_front_porch = 8;
		pinfo->lcdc.v_pulse_width = 1;
		pinfo->lcdc.border_clr = 0;
		pinfo->lcdc.underflow_clr = 0xff;	
		pinfo->lcdc.hsync_skew = 0;

    	ret = platform_device_register(&this_device);
		
		return 0;
	 	}
	}
	msm_fb_add_device(pdev);
	
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = lcdc_panel_probe,
	.driver = {
		.name   = "lcdc_panel_qvga",
	},
};



static int __init lcdc_samsung_panel_init(void)
{
	int ret;

	ret = platform_driver_register(&this_driver);

	if(lcdc_samsung_regist == 0)
	{
		printk("[lxw@lcd&fb]:unregist samsung driver!\n");
		platform_driver_unregister(&this_driver);
		return ret;
	}

	return ret;
}

module_init(lcdc_samsung_panel_init);

