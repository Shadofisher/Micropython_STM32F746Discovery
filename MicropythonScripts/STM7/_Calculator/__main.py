# main.py -- put your code here!
# main.py -- put your code here!
# main.py -- put your code here!

import pyb
import os
from pyb import Pin,LCD,I2C,ExtInt

i2c=I2C(3,I2C.MASTER);

uart=pyb.UART(6,baudrate=115200,read_buf_len=2000);
#tp_int=Pin(Pin.board.LCD_INT,Pin.IN);

t_pressed = 0;

cursor_x = 20;

toggle = 0;

pin1=Pin.board.LCD_INT;

def drawText(text,x,y,colour):
    a = len(text)
    for n in range(a):
        lcd.text(text[n],x+(n*15),y,colour);


def waitTouch():
    global t_pressed;
    global cursor_x;
    pen_colour=0x000000;
    touch = 0;
    regAddressXLow  = 0x04;
    regAddressXHigh = 0x03;
    regAddressYLow  = 0x06;
    regAddressYHigh = 0x05;
    count = 0;
    while(1):
        touch = int(ord(i2c.mem_read(1,0x38,02)))
        if touch == 1:
            #print('Pressed: ',count)
            #print('T_Pressed: ',t_pressed)
            #touch = int(ord(i2c.mem_read(1,0x38,02)))
            #print(touch);
            #if touch == 0 or touch >2:
            #    t_pressed = 0;
            #    print('Pen_UO');
            low = int(ord(i2c.mem_read(1,0x38,regAddressXLow)));
            cord = (low & 0xff); 
            high = int(ord(i2c.mem_read(1,0x38,regAddressXHigh)));
            tempcord = (high & 0x0f) << 8;
            cord=cord | tempcord;
            #print(cord)
            Y=cord; 
            
            low = int(ord(i2c.mem_read(1,0x38,regAddressYLow)));
            cord = (low & 0xff); 
            high = int(ord(i2c.mem_read(1,0x38,regAddressYHigh)));
            tempcord = (high & 0x0f) << 8;
            cord=cord | tempcord;
            #print(cord)
            X=cord; 
            print(X,Y);
            if X>420 and Y>0 and Y < 60:
                    print('Change colour')
                    pen_colour = 0xff0000;

            if X>420 and Y>60 and Y < 120:
                    pen_colour = 0x00ff00;

            if X>420 and Y>120 and Y < 180:
                    pen_colour = 0x000000ff;

#1-C

            if X>60 and X < 96 and Y>172 and Y < 190:
                drawText('1',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>107 and X < 141 and Y>172 and Y < 190:
                drawText('2',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>154 and X < 185 and Y>172 and Y < 190:
                drawText('3',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>60 and X < 96 and Y>209 and Y < 226:
                drawText('4',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>107 and X < 141 and Y>209 and Y < 226:
                drawText('5',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>154 and X < 185 and Y>209 and Y < 226:
                drawText('6',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);


            if X>60 and X < 96 and Y>245 and Y < 261:
                drawText('7',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>107 and X < 141 and Y>245 and Y < 261:
                drawText('8',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>154 and X < 185 and Y>245 and Y < 261:
                drawText('9',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
            if X>200 and X < 233 and Y>245 and Y < 261:
                drawText('0',10+cursor_x,15,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);



            if X>15 and X < 50 and Y>209 and Y < 226:
                drawText('<--',350,20,0x00ffff);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>15 and X < 50 and Y>245 and Y < 261:
                drawText('-->',350,20,0xffff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
            
"""
            if X>67 and X < 84 and Y>124 and Y < 152:
                drawText('2',345+cursor_x,55,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>67 and X < 84 and Y>166 and Y < 198:
                drawText('3',345+cursor_x,55,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

"""


                
            #lcd.thickpixels(X,Y,pen_colour);

def TS_Interrupt():
    global t_pressed;
    t_pressed=1;


def putpixelline(a,y):
    x = 0;
    for n in range(0,1440,3):
        pixel = a[n]<<16 | a[n+1] << 8 | a[n+2];
        lcd.pixels(0+x,y,pixel);
        x = x+1;
        

def displayImage(image):
    file = open(image,'rb')
    for n in range(272):
        a=file.read(1440);
        putpixelline(a,n);





def cursor_toggle():
    global toggle;
    if toggle == 0:
        drawText('_',cursor_x,15,0xffffff);
        toggle=1;
    else:
        drawText('_',cursor_x,15,0x000000);
        toggle=0;
        
    
def sympyfunc(function):
    #clear any garbage in buffer
    while uart.any() > 0:
        uart.read();
    print('done');
    function = function + '\n\r';
    count_w = uart.write(function);
    print(count_w);
    while uart.any() <= count_w:
        pyb.delay(10);
        print(uart.any())

    result = str(uart.read())
    result = result[count_w:];
    
#    res=str(uart.read());
#    res = res.split('\\');
#    res=res[2].split('n');
#    res=res[1];
    result=result.replace("\\n","");
    result=result.replace("\\r","");
    result=result.replace("'","");
    result=result.replace(">","");
    result=result.replace(" ","");
    print(result)
    return(result);    

def rpi(function):
    uart.write(function);
    res=str(uart.read());
    print(res);
    return(res);    
    
    
def pilogin():
    uart.write('\n\r');
    pyb.delay(500);
    login=str(uart.read())

    if  login.find('login') >= 0:
            uart.write('pi\n\r');
            uart.read();
            uart.write('raspberry\n\r');
            uart.read()
            uart.write('python\n\r');
        


tim=pyb.Timer(1);
tim.init(freq=2);
tim.callback(lambda t: cursor_toggle());




#callback = lambda e: TS_Interrupt();
#extint = ExtInt(Pin.board.LCD_INT,ExtInt.IRQ_FALLING,pyb.Pin.PULL_NONE,callback)

lcd=LCD(0);
lcd.clear(0xffff);

#displayImage('python.data');
#drawText('Micropython',10,10,0x0000ff)
#lcd.fillarea(420,0,480,60,0xff0000)
#lcd.fillarea(420,60,480,120,0x00ff00)
#lcd.fillarea(420,120,480,180,0x0000ff)




displayImage('calculator.data')

#waitTouch();


