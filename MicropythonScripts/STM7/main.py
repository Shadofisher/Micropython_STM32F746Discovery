# main.py -- put your code here!
# main.py -- put your code here!
# main.py -- put your code here!
from pyb import Pin,LCD,I2C,ExtInt

i2c=I2C(3,I2C.MASTER);
#tp_int=Pin(Pin.board.LCD_INT,Pin.IN);

t_pressed = 0;


def drawText(text,x,y,colour):
    a = len(text)
    for n in range(a):
        lcd.text(text[n],x+(n*15),y,colour);


def waitTouch():
    global t_pressed;
    touch = 0;
    regAddressXLow  = 0x04;
    regAddressXHigh = 0x03;
    regAddressYLow  = 0x06;
    regAddressYHigh = 0x05;
    count = 0;
    while(1):
        if t_pressed:
            #print('Pressed: ',count);
            touch = int(ord(i2c.mem_read(1,0x38,02)))
            #print(touch);
            if touch == 0:
                t_pressed = 0;
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
            lcd.pixels(X,Y,0xffffff);

def TS_Interrupt():
    global t_pressed;
    t_pressed=1;


callback = lambda e: TS_Interrupt();
extint = ExtInt(Pin.board.LCD_INT,ExtInt.IRQ_FALLING,pyb.Pin.PULL_UP,callback)

lcd=LCD(0);

