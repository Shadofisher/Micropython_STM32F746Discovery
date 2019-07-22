import pyb
from pyb import Pin,LCD,I2C,ExtInt

i2c=I2C(3,I2C.MASTER);


t_pressed = 0

class button:
    def __init__(self,X,Y):
        self.x = X;
        self.y = Y;
    def draw(self):
        lcd.fillarea(self.x,self.y,self.x+ 50,self.y + 50,0xffff80)
        
button1 = button(50,50);
button2 = button(150,150);
button3 = button(350,150);

def drawLine(x0,y0,x1,y1,colour):
    x_interval = abs(x1-x0);
    y_interval = abs(y1-y0);
    if x_interval >= y_interval:
        interval = x_interval
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
            lcd.pixels(new_x,new_y,colour);
        else:
            if (x0>x1):
                signx=-1
            if (y0>y1):
                signy=-1
            new_x = x0 + int(a*(x1-x0)/(interval));
            new_y = (y0+a*signy);
            lcd.pixels(new_x,new_y,colour);

def drawText(text,x,y,colour):
    a = len(text)
    for n in range(a):
        lcd.text(text[n],x+(n*15),y,colour);
        
def waitTouch():
    global t_pressed
    old_x = 0;
    old_y = 0;
    touch = 0
    regAddressXLow  = 0x04
    regAddressXHigh = 0x03
    regAddressYLow  = 0x06
    regAddressYHigh = 0x05
    count = 0
    while(1):
        #old_x = 0;
        #old_y = 0;
        if t_pressed:
            #print('Untouched' +' ' +str(touch))
            touch = int(ord(i2c.mem_read(1,0x38,02)))
            if touch == 1:
                #print('touched a')
                t_pressed = 0;
                low = int(ord(i2c.mem_read(1,0x38,regAddressXLow)));
                cord = (low & 0xff);
                high = int(ord(i2c.mem_read(1,0x38,regAddressXHigh)));
                tempcord = (high & 0x0f) << 8
                cord=cord | tempcord;
                Y=cord
                low = int(ord(i2c.mem_read(1,0x38,regAddressYLow)))
                cord = (low & 0xff);
                high = int(ord(i2c.mem_read(1,0x38,regAddressYHigh)))
                tempcord = (high & 0x0f) << 8
                cord=cord | tempcord
                X=cord;
                lcd.pixels(X,Y,0xffffff);
                if old_x == 0:
                    old_x = X;
                if old_y == 0:
                    old_y = Y;
                drawLine(old_x,old_y,X,Y,0xffffff);
                lcd.fillarea(300,10,450,100,0)
                drawText(' '+str(X) + '  ' +str(Y),300,10,0x00ff00)
                if  X > button1.x and X < (button1.x +50) and Y > button1.y and Y < (button1.y +50):
                    drawText('Touch',356,231,0x0000ff);
                if  X > button2.x and X < (button2.x +50) and Y > button2.y and Y < (button2.y +50):
                    drawText('Touch',356,231,0xff0000);
                if  X > button3.x and X < (button3.x +50) and Y > button3.y and Y < (button3.y +50):
                    drawText('Touch',356,231,0x00ff00);
                old_x = X;
                old_y = Y;
            else:
                old_x = 0;
                old_y = 0;
                
                

def TS_Interrupt():
    global t_pressed;
    t_pressed=1;
callback = lambda e: TS_Interrupt()
extint = ExtInt(Pin.board.LCD_INT,ExtInt.IRQ_FALLING,pyb.Pin.PULL_UP,callback)



lcd=LCD(0)
lcd=LCD(0);
lcd.clear(0);
drawText("Sketch Pad",150,10,0x00ff00)
button1.draw();
button2.draw();
button3.draw();
#drawLine(10,10,70,70,0xffffff)
waitTouch();
