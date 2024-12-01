const CLK = 1;
const CS = 0;
const DIN = 10;

function spiWrite(val){
    digitalWrite(CS, false);

    for(var i = 15; i >= 0; i--){
        var vtw = ((val >> i) & 1) !== 0;
        digitalWrite(DIN, vtw);
        digitalWrite(CLK, true);
        digitalWrite(CLK, false);
    }

    digitalWrite(CS, true);
}

function matrixSend(addr, data){
    spiWrite(((addr & 0x0f) << 8) | (data & 0xff));
}

const smiley = [
    0b00000000,
    0b01100100,
    0b01100010,
    0b00000010,
    0b00000010,
    0b01100010,
    0b01100100,
    0b00000000
];

const nsu = [
    0b01000000,
    0b11100000,
    0b01000000,
    0b00011110,
    0b00000100,
    0b00001000,
    0b00011110,
    0b00000000
]

function drawImage(img){
    for(var y = 0; y < 8; y++){
        matrixSend(y+1, img[y]);
    }
}

function imgBtn(name, data, x, y){
    var btn = new LVGLButton(function (){
        print('Button pressed, drawing ' + name);
        drawImage(data);
    })
    btn.text = name;
    btn.x = x;
    btn.y = y;
}

pinMode(CLK, 0);
pinMode(CS, 0);
pinMode(DIN, 0);
digitalWrite(CS, true);
digitalWrite(CLK, false);

matrixSend(0x09, 0); // no decode
matrixSend(0x0C, 1); // no shutdown
matrixSend(0x0b, 0b111); // no scan-limit
matrixSend(0x0a, 1); // intensity

imgBtn("smiley", smiley, 10, 10);
imgBtn("N*", nsu, 50, 30);
