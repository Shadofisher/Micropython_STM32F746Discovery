# main.py -- put your code here!
# main.py -- put your code here!
# main.py -- put your code here!

import pyb
import os
from pyb import Pin,LCD,I2C,ExtInt

i2c=I2C(3,I2C.MASTER);
#tp_int=Pin(Pin.board.LCD_INT,Pin.IN);

t_pressed = 0;

cursor_x = 20;

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

            if X>44 and X < 74 and Y>149 and Y < 180:
                drawText('1',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>115 and X < 144 and Y>149 and Y < 180:
                drawText('2',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                

            if X>184 and X < 215 and Y>149 and Y < 180:
                drawText('3',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>253 and X < 284 and Y>149 and Y < 180:
                drawText('A',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>324 and X <354 and Y>149 and Y < 180:
                drawText('B',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>394 and X <424 and Y>149 and Y < 180:
                drawText('C',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
#4-f

            if X>44 and X < 74 and Y>190 and Y < 210:
                drawText('4',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>115 and X < 144 and Y>190 and Y < 210:
                drawText('5',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                

            if X>184 and X < 215 and Y>190 and Y < 210:
                drawText('6',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>253 and X < 284 and Y>190 and Y < 210:
                drawText('D',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>324 and X <354 and Y>190 and Y < 210:
                drawText('E',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>394 and X <424 and Y>190 and Y < 210:
                drawText('F',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
#7-=
            if X>44 and X < 74 and Y>228 and Y < 250:
                drawText('7',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>115 and X < 144 and Y>228 and Y < 250:
                drawText('8',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                

            if X>184 and X < 215 and Y >228 and Y < 250:
                drawText('9',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>253 and X < 284 and Y>228 and Y < 250:
                drawText('+',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>324 and X <354 and Y>228 and Y < 250:
                drawText('-',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>394 and X <424 and Y>228 and Y < 250:
                drawText('=',20+cursor_x,20,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);



                
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




#callback = lambda e: TS_Interrupt();
#extint = ExtInt(Pin.board.LCD_INT,ExtInt.IRQ_FALLING,pyb.Pin.PULL_NONE,callback)

lcd=LCD(0);
lcd.clear(0xffff);

#displayImage('python.data');
#drawText('Micropython',10,10,0x0000ff)
#lcd.fillarea(420,0,480,60,0xff0000)
#lcd.fillarea(420,60,480,120,0x00ff00)
#lcd.fillarea(420,120,480,180,0x0000ff)




#displayImage('calculator.data')

#waitTouch();


