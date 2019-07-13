# main.py -- put your code here!
# main.py -- put your code here!
# main.py -- put your code here!

import pyb
import os
from pyb import Pin,LCD,I2C,ExtInt
import mystack;

stk=mystack.Stack();
red_function_shift=0;

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

cursor_x = 35;

toggle = 0;

pin1=Pin.board.LCD_INT;

stack = [];
current_entry='';



def drawText(text,x,y,colour):
    text=text.replace("**",'^')
    a = len(text)
    for n in range(a):
        lcd.text(text[n],x+(n*15),y,colour);


def calc_init():    
    lcd.fillarea(16,14,462,117,LCD_COLOUR);
    drawText(' >',STK_X,STK1_Y,0);
    drawText('1:',STK_X,STK2_Y,0);
    drawText('2:',STK_X,STK3_Y,0);
    drawText('3:',STK_X,STK4_Y,0);


def lcd_showstack():
    global red_function_shift;
    lcd.fillarea(16,14,462,117,LCD_COLOUR);
        
    calc_init();
    n = stk.get_index();
    if n > 3:
        n=3;
    for r in range(1,(n+1)):
        drawText(stk.get((stk.get_index())-r),45,97-26-((r-1)*26),0x00)
    
    if red_function_shift ==1:
        lcd.fillarea(448,15,458,25,0xff0000);
    




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
    global red_function_shift;
    pen_colour=0x000000;
    touch = 0;
    regAddressXLow  = 0x04;
    regAddressXHigh = 0x03;
    regAddressYLow  = 0x06;
    regAddressYHigh = 0x05;
    count = 0;
    calc_init();
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
                    pen_colour = 0x00;

            if X>420 and Y>120 and Y < 180:
                    pen_colour = 0x000000ff;

#1-C

            if X>60 and X < 96 and Y>172 and Y < 190:
                drawText('1',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '1';

#pi            
            if X>56 and X < 99 and Y>130 and Y < 160:
                drawText('pi',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + 'pi';


            if X>107 and X < 141 and Y>172 and Y < 190:
                drawText('2',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '2';

            if X>154 and X < 185 and Y>172 and Y < 190:
                drawText('3',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '3';

            if X>60 and X < 96 and Y>209 and Y < 226:
                drawText('4',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '4';


            if X>107 and X < 141 and Y>209 and Y < 226:
                drawText('5',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '5';

            if X>154 and X < 185 and Y>209 and Y < 226:
                drawText('6',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '6';


            if X>60 and X < 96 and Y>245 and Y < 261:
                drawText('7',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '7';

            if X>107 and X < 141 and Y>245 and Y < 261:
                drawText('8',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '8';

            if X>154 and X < 185 and Y>245 and Y < 261:
                drawText('9',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '9';

            if X>200 and X < 233 and Y>245 and Y < 261:
                drawText('0',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '0';

            if X>200 and X < 234 and Y>209 and Y < 227:
                drawText('.',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + '.';


            if X>384 and X < 420 and Y>209 and Y < 227:
                drawText('x',STK_X+10+cursor_x,STK1_Y,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);
                current_entry = current_entry + 'x';


#Squaroot
            if X>101 and X < 146 and Y>133 and Y < 160:   
                valid_function=0;
                print('Index',stk.get_index())    
                if stk.get_index() >= 1:
                    value1 = stk.pop();  
                    function='sqrt(' +value1 +')';
                    valid_function=1;
                elif len(current_entry) >0:
                    function='sqrt(' + current_entry +')'
                    valid_function=1;
                    
                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();

                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();


#SWAP
            if X>374 and X < 418 and Y>133 and Y < 160:
                lcd_showstack();
                if stk.get_index() >= 2:    
                    value1=stk.pop();                
                    value2=stk.pop();
                    stk.push(value1);                
                    stk.push(value2);                
                    current_entry='';
                    pyb.delay(200);
                    cursor_x=35;
                    lcd_showstack();
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();

                



#CLR
            if X>15 and X < 50 and Y>137 and Y < 154:
                lcd_showstack();
                current_entry='';
                pyb.delay(200);
                cursor_x=35;


#drop
            if X>424 and X < 460 and Y>137 and Y < 154:
                stk.pop();
                lcd_showstack();
                pyb.delay(200);
                if len(current_entry) > 0:
                    drawText(current_entry,STK_X+10+35,STK1_Y,0x00)



#SOLVESET
            if X>328 and X < 375 and Y>132 and Y < 159:
                if red_function_shift == 1:
                    valid_function=0;   
                    print('Index',stk.get_index())    
                    if stk.get_index() >= 1:
                        value1 = stk.pop();  
                        function='solveset(' +value1 +',x)';
                        valid_function=1;

                    if valid_function == 1:
                        print(function)
                        lcd_showstack();
                        drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                        answer = sympyfunc(function);
                        stk.push(answer)                    
                        lcd_showstack();
                        pyb.delay(200);
                        current_entry = '';
                        cursor_x = 35;
                    else:
                        drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                        pyb.delay(1000);
                        lcd_showstack();


#Differentiate
            if X>330 and X < 377 and Y>166 and Y < 196:   
                valid_function=0;
                print('Index',stk.get_index())    
                if stk.get_index() >= 1:
                    value1 = stk.pop();  
                    function='diff(' +value1 +',x)';
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();



#SIMPLIFY
            if X>285 and X < 326 and Y>130 and Y < 161:   
                valid_function=0;
                print('Index',stk.get_index())    
                if stk.get_index() >= 1:
                    value1 = stk.pop();  
                    function='simplify(' +value1 +')';
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();


#INTEGRATE
            if X>338 and X < 375 and Y>209 and Y < 225:   
                valid_function=0;
                print('Index',stk.get_index())    
                if stk.get_index() >= 1:
                    value1 = stk.pop();  
                    function='integrate(' +value1 +',x)';
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();


#EXPAND
            if X>328 and X < 375 and Y>132 and Y < 159:
                if red_function_shift == 0:   
                    valid_function=0;
                    print('Index',stk.get_index())    
                    if stk.get_index() >= 1:
                        value1 = stk.pop();  
                        function='expand(' +value1 +')';
                        valid_function=1;
                    if valid_function == 1:
                        print(function)
                        lcd_showstack();
                        drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                        answer = sympyfunc(function);
                        stk.push(answer)                    
                        lcd_showstack();
                        pyb.delay(200);
                        current_entry = '';
                        cursor_x = 35;
                    else:
                        drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                        pyb.delay(1000);
                        lcd_showstack();


#EVALUATE
            if X>431 and X < 466 and Y>172 and Y < 193:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    cursor_x = 25;
                    function='N('+current_entry +',4)';
                    valid_function=1;
                elif stk.get_index() >= 1:
                    value1 = stk.pop();  
                    function='N('+value1 +',4)';
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();



#TAN
            if X>384 and X < 420 and Y>244 and Y < 261:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    cursor_x = 25;
                    function='tan('+current_entry +')';
                    valid_function=1;
                elif stk.get_index() >= 1:
                    value1 = stk.pop();  
                    function='tan('+value1 +')';
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();



#COS
            if X>337 and X < 372 and Y>244 and Y < 261:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    cursor_x = 25;
                    function='cos('+current_entry +')';
                    valid_function=1;
                elif stk.get_index() >= 1:
                    value1 = stk.pop();  
                    function='cos('+value1 +')';
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();


#SIN
            if X>292 and X < 326 and Y>244 and Y < 261:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    cursor_x = 25;
                    if red_function_shift==0:
                        function='sin('+current_entry +')';
                    else:
                        function='asin('+current_entry +')';
                    valid_function=1;
                elif stk.get_index() >= 1:
                    value1 = stk.pop();  
                    if red_function_shift==0:
                        function='sin('+value1 +')';
                    else:
                        function='asin('+value1 +')';
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();

#To the power of
            if X>292 and X < 326 and Y>209 and Y < 226:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    value = stk.pop();  
                    cursor_x = 25;
                    function=value + '**' +current_entry;
                    valid_function=1;
                elif stk.get_index() >= 2:
                    value1 = stk.pop();
                    value2 = stk.pop();    
                    function=value2 + '**'+value1;                
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();




#Division
            if X>245 and X < 281 and Y>172 and Y < 190:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    value = stk.pop();  
                    cursor_x = 25;
                    function='('+value+')' + '/' +'('+current_entry+')';
                    valid_function=1;
                elif stk.get_index() >= 2:
                    value1 = stk.pop();
                    value2 = stk.pop();    
                    function='(' +value2 +')' + '/(' +value1 +')';                
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();





#Multiplication
            if X>200 and X < 235 and Y>172 and Y < 190:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    value = stk.pop();  
                    cursor_x = 25;
                    function='(' + value +')' + '*' + current_entry;
                    valid_function=1;
                elif stk.get_index() >= 2:
                    value1 = stk.pop();
                    value2 = stk.pop();    
                    function='(' + value2 + ')' + '*' + '(' + value1 + ')';                
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    

                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();



#Subtraction
            if X>245 and X < 280 and Y>210 and Y < 226:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:

                    value = stk.pop();  
                    cursor_x = 25;
                    function=value + '-' +current_entry;
                    valid_function=1;
                elif stk.get_index() >= 2:
                    value1 = stk.pop();
                    value2 = stk.pop();    
                    function=value2 + '-'+value1;                
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();


#Addition
            if X>245 and X < 280 and Y>244 and Y < 263:   
                valid_function=0;
                print('Index',stk.get_index())    
                if len(current_entry) > 0:
                    value = stk.pop();  
                    cursor_x = 25;
                    function=value + '+' +current_entry;
                    valid_function=1;
                elif stk.get_index() >= 2:
                    value1 = stk.pop();
                    value2 = stk.pop();    
                    function=value1 + '+'+value2;                
                    valid_function=1;

                if valid_function == 1:
                    print(function)
                    lcd_showstack();
                    drawText('wait...',STK_X+50,STK1_Y,0x0000ff);    
                    answer = sympyfunc(function);
                    stk.push(answer)                    
                    lcd_showstack();
                    pyb.delay(200);
                    current_entry = '';
                    cursor_x = 35;
                else:
                    drawText('<error>',STK_X+50,STK1_Y,0x0000ff);
                    pyb.delay(1000);
                    lcd_showstack();


            if X>419 and X < 478 and Y>235 and Y < 270:
                if len(current_entry) > 0:                
                    stk.push(current_entry);
                    lcd_showstack();
                #stack.append(current_entry)
                #drawText(':' + current_entry,10,40,0x00);
                #cursor_x = cursor_x+15;
                #pyb.delay(200);
                #res=sympyfunc('1.0/' +stack.pop())
                #drawText(res,50,40,0x00);
                cursor_x = 25;

                current_entry ='';


            #if X>15 and X < 50 and Y>209 and Y < 226:
            #    drawText('<--',350,20,0x00ffff);
            #    cursor_x = cursor_x+15;
            #    pyb.delay(200);

            if X>5 and X < 56 and Y>203 and Y < 232:
                if red_function_shift == 0:
                    lcd.fillarea(448,15,458,25,0xff0000);
                    red_function_shift = 1;
                else:       
                    lcd.fillarea(448,15,458,25,LCD_COLOUR);
                    red_function_shift = 0;
                pyb.delay(200);
            
"""
            if X>67 and X < 84 and Y>124 and Y < 152:
                drawText('2',345+cursor_x,55,0x00);
                cursor_x = cursor_x+15;
                pyb.delay(200);

            if X>67 and X < 84 and Y>166 and Y < 198:
                drawText('3',345+cursor_x,55,0x00);
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
        
            uart.read()
            uart.write('from sympy import *\n\r');

            uart.read()
            uart.write('from sympy import *\n\r');

            pyb.delay(5000);

            uart.read()
            uart.write('x,t=symbols(\'x y\')\n\r');

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
#lcd.fillarea(420,60,480,120,0x00)
#lcd.fillarea(420,120,480,180,0x0000ff)


pilogin();

displayImage('calculator.data')

waitTouch();



