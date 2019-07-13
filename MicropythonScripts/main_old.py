# main.py -- put your code here!
from pyb import LCD,SPI
import math
import os
import sys
import random
SPI(5,SPI.MASTER,6000000)
lcd=LCD(0)
lcd.line(0x1c,1,1,1)
led=pyb.LED(1)


   

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










