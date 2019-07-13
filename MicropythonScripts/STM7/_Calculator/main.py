# main.py -- put your code here!
# main.py -- put your code here!
# main.py -- put your code here!

import pyb
import os
from pyb import Pin,LCD,I2C,ExtInt
import mystack;

stk=mystack.Stack();



i2c=I2C(3,I2C.MASTER);
LCD_COLOUR=0x4F5536

STK_X=18;
STK1_Y=97;
STK2_Y=STK1_Y-26;
STK3_Y=STK2_Y-26;
STK4_Y=STK3_Y-26;

uart=pyb.UART(6,baudrate=115200,read_buf_len=2000);
#tp_int=Pin(Pin.board.LCD_INT,Pin.IN);

t_pressed = 0;

cursor_x = 20;

toggle = 0;

pin1=Pin.board.LCD_INT;

stack = [];
current_entry='';



def drawText(text,x,y,colour):
    a = len(text)
    for n in range(a):
        lcd.text(text[n],x+(n*15),y,colour);


def calc_init():    
    lcd.fillarea(16,14,462,117,LCD_COLOUR);
    drawText('->:',STK_X,STK1_Y,0);
    drawText('2:',STK_X,STK2_Y,0);
    drawText('3:',STK_X,STK3_Y,0);
    drawText('4:',STK_X,STK4_Y,0);


def lcd_showstack():
    lcd.fillarea(16,14,462,117,LCD_COLOUR);
    calc_init();
    n = stk.get_index();
    if n > 3:
        n=3;
    for r in range(1,(n+1)):
        drawText(stk.get((stk.get_index())-r),45,97-26-((r-1)*26),0x00)
    
    




def sympyfunc(function):
    #clear any garbage in buffer
    while uart.any() > 0:
        uart.read();
    #print('done');
    function = function + '\n\r';
    count_w = uart.write(function);
    #print(count_w);
    while uart.any() <= count_w:
        pyb.delay(10);
        #print(uart.any())

    result = str(uart.read())
    result = result[count_w:];    
    result=result.replace("\\n","");
    result=result.replace("\\r","");
    result=result.replace("'","");
    result=result.replace(">","");
    result=result.replace(" ","");
    print(result)
    return(result);    



def waitTouch():
    global t_pressed;
    global cursor_x;
    global current_entry;
    global stack;
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
            #print(X,Y);
            if X>420 and Y>0 and Y < 60:
                    print('Change colour')
                    pen_colour = 0xff0000;

            if X>420 and Y>60 and Y < 120:
                    pen_colour = 0x00ff00;

            if X>420 and Y>120 and Y < 180:
                    pen_colour = 0x000000ff;

#1-C

            if X>60 and X < 96 and Y>172 and Y < 190:
                drawText('1',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '1';

            if X>107 and X < 141 and Y>172 and Y < 190:
                drawText('2',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '2';

            if X>154 and X < 185 and Y>172 and Y < 190:
                drawText('3',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '3';

            if X>60 and X < 96 and Y>209 and Y < 226:
                drawText('4',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '4';

            if X>107 and X < 141 and Y>209 and Y < 226:
                drawText('5',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '5';

            if X>154 and X < 185 and Y>209 and Y < 226:
                drawText('6',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '6';


            if X>60 and X < 96 and Y>245 and Y < 261:
                drawText('7',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '7';

            if X>107 and X < 141 and Y>245 and Y < 261:
                drawText('8',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '8';

            if X>154 and X < 185 and Y>245 and Y < 261:
                drawText('9',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '9';

            if X>200 and X < 233 and Y>245 and Y < 261:
                drawText('0',STK_X+10+cursor_x,STK1_Y,0x00ff00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '0';


            if X>430 and X < 465 and Y>244 and Y < 263:
                value = stk.pop();  
                cursor_x = 20;
                function=value + '+' +current_entry;
                print(function)
                lcd_showstack();
                drawText('wait...',STK_X+30,STK1_Y,0x0000ff);    
                answer = sympyfunc(function);
                stk.push(answer)                    
                lcd_showstack();
                #drawText(answer,STK_X+30,STK1_Y,0x0000ff);
                pyb.delay(200);
                current_entry = '';
                cursor_x = 20;



            if X>15 and X < 50 and Y>245 and Y < 261:
                if len(current_entry) > 0:                
                    stk.push(current_entry);
                    lcd_showstack();
                #stack.append(current_entry)
                #drawText(':' + current_entry,10,40,0x00ff00);
                #cursor_x = cursor_x+15;
                #pyb.delay(200);
                #res=sympyfunc('1.0/' +stack.pop())
                #drawText(res,50,40,0x00ff00);
                
                current_entry ='';


            #if X>15 and X < 50 and Y>209 and Y < 226:
            #    drawText('<--',350,20,0x00ffff);
            #    cursor_x = cursor_x+15;
            #    pyb.delay(200);

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
        drawText('_',cursor_x,STK1_Y,0x00);
        toggle=1;
    else:
        drawText('_',cursor_x,STK1_Y,LCD_COLOUR);
        toggle=0;
        
    
def sympyfunc(function):
    #clear any garbage in buffer
    while uart.any() > 0:
        uart.read();
    #print('done');
    function = function + '\n\r';
    count_w = uart.write(function);
    #print(count_w);
    while uart.any() <= count_w:
        pyb.delay(10);
        #print(uart.any())

    result = str(uart.read())
    result = result[count_w:];    
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
        


#tim=pyb.Timer(1);
#tim.init(freq=2);
#tim.callback(lambda t: cursor_toggle());




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


