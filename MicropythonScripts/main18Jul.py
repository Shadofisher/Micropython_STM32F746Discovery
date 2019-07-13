# main.py -- put your code here!
from pyb import LCD,SPI,I2C
import math
import os
import sys
import random
import pyb
SPI(5,SPI.MASTER,6000000)
lcd=LCD(0)
lcd.line(0x1c,1,1,1)
led=pyb.LED(1)

i2c=pyb.I2C(3,pyb.I2C.MASTER)

def mandelbrot():
    # returns True if c, complex, is in the Mandelbrot set
    #@micropython.native
    def in_set(c):
        z = 0
        for i in range(40):
            z = z*z + c
            if abs(z) > 60:
                return False
        return True

    lcd.line(0xffff,1,1,1)
    for u in range(230):
        for v in range(310):
            if in_set((u / 30 - 2) + (v / 15 - 1) * 1j):
                lcd.pixels(3*u, 3*v,0)


def i2cinit():
    i2c.init(baudrate=50000)
    print(i2c.scan())     


def drawLine(x0,y0,x1,y1,colour):
    x_interval = abs(x1-x0); 
    y_interval = abs(y1-y0);

    if x_interval >= y_interval:
        interval = x_interval;
    else:
        interval = y_interval;
  
    for a in range((interval)):
        #print(a);
        #print(interval);
        signx=1;
        signy=1;
        if x_interval >= y_interval:
            if (x0>x1):
                signx=-1
            if (y0>y1):
                signy=-1
            new_y = y0 + int(a*(y1-y0)/(interval));
            new_x = (x0+a*signx);
            #print('New_X',new_x);
            #print('New_y',new_y);
                
            led.toggle()
            lcd.pixels(new_x,new_y,colour);
            #print('X greater')
        else:
            if (x0>x1):
                signx=-1
            if (y0>y1):
                signy=-1
            #new_x = x0+int((signx*a*abs(x1-x0)/(interval)));
            new_x = x0 + int(a*(x1-x0)/(interval));
            new_y = (y0+a*signy);
            #print('New_X',new_x);
            #print('New_y',new_y);

            led.toggle()    
            lcd.pixels(new_x,new_y,colour);
            #print('Y greate')
            #print(signx)
            #print(signy)



def drawSquare(x,y,colour):
    x0=120-int(x/2);
    x1=120+int(x/2);
    y0=160-int(y/2);
    y1=160+int(y/2);
    drawLine(x0,y0,x0,y1,colour);
    drawLine(x0,y1,x1,y1,colour);
    drawLine(x1,y1,x1,y0,colour);
    drawLine(x1,y0,x0,y0,colour);


def animateSquare(colour):
    for r in range(1,140,15):
        drawSquare(r,r,colour)


def animateLine():
    a=0;
    for r in range(320):
        drawLine(0,r,240,r,r)

def animateClear(colour):
    a=0;
    for r in range(320):
        drawLine(0,r,240,r,colour)
def drawParabola(colour):
    a=0;
    x0 = 0;
    y0 = 0;
    for r in range(0,80,4):
        y=int(math.pow(r/4,2));
        lcd.pixels(r,y,colour)
        drawLine(x0,y0,r,y,colour);
        x0=r;
        y0=y;

    
def drawPicture():
    file=open('test.data','rb');
    for y in range(100):
        for n in range(100):
            byte1 = file.read(1);
            byte2 = file.read(1);
            byte3 = file.read(1);
            lcd.pixels(n,y,int(byte1[0]))

    file.close();


def textBox(x,y,x0,y0,colour):
    lcd.fillarea(x,y,x0,y0,0)
    lcd.fillarea(x+5,y+5,x0-5,y0-5,colour)


def drawText(text,x,y,colour):
    a = len(text)
    for n in range(a):
        lcd.text(text[n],x+(n*10),y,colour);

def drawRandomText(text,count):
    for n in range(count):
        x=random.randrange(20,200);
        y=random.randrange(60,300);
        colour=random.randrange(0,0x8fff);
        drawText(text,x,y,colour);

def drawRandomBlocks(count):
    for n in range(count):
        x=random.randrange(20,200);
        y=random.randrange(60,300);
        colour=random.randrange(0,0x8fff);
        lcd.fillarea(x,y,x+20,y+20,colour);
        
def drawCounter(x,y):
    #lcd.line(0x1c,1,1,1);    
    for n in range(1001):
        drawText(str(n),x,y,0x0000);
        pyb.delay(10);
        drawText(str(n),x,y,0xffff);

def drawHWLED(interval_on,interval_off,count):
    for n in range(count):
        led.on();
        pyb.delay(interval_on);
        led.off();
        pyb.delay(interval_off);
def drawSWLED(interval_on,interval_off,count):    
    for n in range(count):
        lcd.fillarea(120,160,130,170,0xffff);
        pyb.delay(interval_on);
        lcd.fillarea(120,160,130,170,0x0000);
        pyb.delay(interval_off);

def demoBlock(count,delay):
    for n in range(count):
        lcd.fillarea(120-n,160-n,120+n,160+n,0x0220);
        pyb.delay(delay);

"""
lcd.line(0x1c,1,1,1);
drawText('Counter:',20,20,0xffff);		
textBox(80,130,150,160,0xffff);
#drawText('1234',95,136,0x0000);
drawCounter(95,136);
    
#drawText('G',100,100,0)
#drawText('0',100,120,0)
#drawRandomText('Micro',30);		        
#drawRandomText('Python',30);		
drawText('MicrpPython:',20,20,0xffff);
drawText('Version: 1.0a',20,40,0xffff);
pyb.delay(2000);
lcd.line(0x1c,1,1,1);
drawText('Random Pixels:',20,20,0xffff);
drawRandomText('.',5000);
pyb.delay(2000);

lcd.line(0x1c,1,1,1);
drawText('Random blocks:',20,20,0xffff);		
drawRandomBlocks(100);		
pyb.delay(2000);

lcd.line(0x1c,1,1,1);
drawText('Random Text:',20,20,0xffff);
drawRandomText('Python',50);
pyb.delay(2000);

lcd.line(0x1c,1,1,1);
drawText('LED: HW',20,20,0xffff);
drawHWLED(300,100,10);

lcd.line(0x1c,1,1,1);
drawText('LED: SW',20,20,0xffff);
drawSWLED(300,100,10);


lcd.line(0x1c,1,1,1);
drawText('Animate: Square',20,20,0xffff);
animateSquare(0xffff);
pyb.delay(200);
animateSquare(0x0000);
pyb.delay(1000);

lcd.line(0x1c,1,1,1);
drawText('Expand block(100ms):',20,20,0xffff);		
demoBlock(80,100);		
pyb.delay(2000);
lcd.line(0x1c,1,1,1);
drawText('Expand block(10ms):',20,20,0xffff);		
demoBlock(80,10);		
pyb.delay(2000);

lcd.line(0x1c,1,1,1);
drawText('Expand block(0ms):',20,20,0xffff);		
demoBlock(80,0);		
pyb.delay(2000);

lcd.line(0x1c,1,1,1);
drawText('Counter:',20,20,0xffff);		
textBox(80,130,150,160,0xffff);
#drawText('1234',95,136,0x0000);
drawCounter(95,136);


lcd.line(0x1c,1,1,1);
drawText('D',20,20,0xffff);
pyb.delay(100);
drawText('o',30,20,0xffff);
pyb.delay(100);
drawText('n',40,20,0xffff);
pyb.delay(100);
drawText('e',50,20,0xffff);
pyb.delay(100);
drawText('.',60,20,0xffff);
pyb.delay(100);
drawText('.',70,20,0xffff);
pyb.delay(100);
drawText('.',80,20,0xffff);


"""




STMPE811_ADDRESS=0x41
STMPE811_CHIP_ID_VALUE=0x0811	#Chip ID
STMPE811_CHIP_ID=0x00	        #STMPE811 Device identification
STMPE811_ID_VER=0x02	        #STMPE811 Revision number; =0x01 for engineering sample; =0x03 for final silicon
STMPE811_SYS_CTRL1=0x03	        #Reset control
STMPE811_SYS_CTRL2=0x04	        #Clock control
STMPE811_SPI_CFG=0x08	        #SPI interface configuration
STMPE811_INT_CTRL=0x09	        #Interrupt control register
STMPE811_INT_EN=0x0A	        #Interrupt enable register
STMPE811_INT_STA=0x0B	        #Interrupt status register
STMPE811_GPIO_EN=0x0C	        #GPIO interrupt enable register
STMPE811_GPIO_INT_STA=0x0D	#GPIO interrupt status register
STMPE811_ADC_INT_EN=0x0E	#ADC interrupt enable register
STMPE811_ADC_INT_STA=0x0F	#ADC interface status register
STMPE811_GPIO_SET_PIN=0x10	#GPIO set pin register
STMPE811_GPIO_CLR_PIN=0x11	#GPIO clear pin register
STMPE811_MP_STA=0x12	        #GPIO monitor pin state register
STMPE811_GPIO_DIR=0x13	        #GPIO direction register
STMPE811_GPIO_ED=0x14	        #GPIO edge detect register
STMPE811_GPIO_RE=0x15	        #GPIO rising edge register
STMPE811_GPIO_FE=0x16	        #GPIO falling edge register
STMPE811_GPIO_AF=0x17	        #alternate function register
STMPE811_ADC_CTRL1=0x20	        #ADC control
STMPE811_ADC_CTRL2=0x21	        #ADC control
STMPE811_ADC_CAPT=0x22	        #To initiate ADC data acquisition
STMPE811_ADC_DATA_CHO=0x30	#ADC channel 0
STMPE811_ADC_DATA_CH1=0x32	#ADC channel 1
STMPE811_ADC_DATA_CH2=0x34	#ADC channel 2
STMPE811_ADC_DATA_CH3=0x36	#ADC channel 3
STMPE811_ADC_DATA_CH4=0x38	#ADC channel 4
STMPE811_ADC_DATA_CH5=0x3A	#ADC channel 5
STMPE811_ADC_DATA_CH6=0x3C	#ADC channel 6
STMPE811_ADC_DATA_CH7=0x3E	#ADC channel 7
STMPE811_TSC_CTRL=0x40	        #4-wire touchscreen controller setup
STMPE811_TSC_CFG=0x41	        #Touchscreen controller configuration
STMPE811_WDW_TR_X=0x42	        #Window setup for top right X
STMPE811_WDW_TR_Y=0x44	        #Window setup for top right Y
STMPE811_WDW_BL_X=0x46	        #Window setup for bottom left X
STMPE811_WDW_BL_Y=0x48	        #Window setup for bottom left Y
STMPE811_FIFO_TH=0x4A	        #FIFO level to generate interrupt
STMPE811_FIFO_STA=0x4B	        #Current status of FIFO
STMPE811_FIFO_SIZE=0x4C	        #Current filled level of FIFO
STMPE811_TSC_DATA_X=0x4D	#Data port for touchscreen controller data access
STMPE811_TSC_DATA_Y=0x4F	#Data port for touchscreen controller data access
STMPE811_TSC_DATA_Z=0x51	#Data port for touchscreen controller data access
STMPE811_TSC_DATA_XYZ=0x52	#Data port for touchscreen controller data access
STMPE811_TSC_FRACTION_Z=0x56	#Touchscreen controller FRACTION_Z
STMPE811_TSC_DATA=0x57	        #Data port for touchscreen controller data access
STMPE811_TSC_I_DRIVE=0x58	#Touchscreen controller drivel
STMPE811_TSC_SHIELD=0x59	#Touchscreen controller shield
STMPE811_TEMP_CTRL=0x60	        #Temperature sensor setup
STMPE811_TEMP_DATA=0x61	        #Temperature data access port
STMPE811_TEMP_TH=0x62	        #Threshold for temperature controlled interrupt
STMPE811_I2C=0

def TM_I2C_Write(null,address, register, value):
    print(null,address,register,value);
    i2c.mem_write(value,address,register);

def TM_STMPE811_Read(address,reg):
       
    return(i2c.mem_read(1,address,reg));

def TMInit():
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_SYS_CTRL1, 0x02);
    pyb.delay(5);
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_SYS_CTRL1, 0x00);
    pyb.delay(2);
    # Get the current register value */
    mode = int(hex(ord(TM_STMPE811_Read(STMPE811_ADDRESS,STMPE811_SYS_CTRL2))));
    print(mode)
    mode &= ~(0x01);
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_SYS_CTRL2, mode);
    mode = int(hex(ord(TM_STMPE811_Read(STMPE811_ADDRESS,STMPE811_SYS_CTRL2))));
    mode &= ~(0x02); 
    print(mode)
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_SYS_CTRL2, mode); 
    # Select Sample Time, bit number and ADC Reference */
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_ADC_CTRL1, 0x49);
    # Wait for 2 ms */
    pyb.delay(2); 
    # Select the ADC clock speed: 3.25 MHz */
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_ADC_CTRL2, 0x01);
    # Select TSC pins in non default mode */  
    mode = int(hex(ord(TM_STMPE811_Read(STMPE811_ADDRESS,STMPE811_GPIO_AF))));
    mode |= 0x1E;
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_GPIO_AF, mode);
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_TSC_CFG, 0x9A);
    # Configure the Touch FIFO threshold: single point reading */
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_FIFO_TH, 0x01);
    # Clear the FIFO memory content. */
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_FIFO_STA, 0x01);
    # Put the FIFO back into operation mode  */
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_FIFO_STA, 0x00);
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_TSC_FRACTION_Z, 0x01);
    # Set the driving capability (limit) of the device for TSC pins: 50mA
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_TSC_I_DRIVE, 0x01);
    # Touch screen control configuration (enable TSC):
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_TSC_CTRL, 0x03);
    # Clear all the status pending bits if any */
    TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_INT_STA, 0xFF);
    """
    # Enable global interrupts */
    #
    mode = TM_I2C_Read(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_INT_CTRL);
	mode |= 0x01;
	TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_INT_CTRL, mode);
	*/
	# Enable touch interrupt */
	#
	mode = TM_I2C_Read(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_INT_EN);
	mode |= 0x01;
	TM_I2C_Write(STMPE811_I2C, STMPE811_ADDRESS, STMPE811_INT_EN, mode);
	*/
	pyb.Delay(2);
	"""
	
def readTouch():
    	val = TM_STMPE811_Read(STMPE811_ADDRESS,STMPE811_TSC_CTRL);
        print(val);





i2cinit();
TMInit();
	

    











    


