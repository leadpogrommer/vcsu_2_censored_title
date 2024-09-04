#include <SPI.h>
#include <stdint.h>

#define CS 10

uint8_t snake_coords [64][2] = {{4, 5}};
uint8_t snake_start = 0;
uint8_t snake_end = 1;
uint8_t field[8][8];

uint8_t direction_ports[] = {4, 3, 2, A7};
int8_t velocity[] = {1, 0};
uint8_t food_coords[] = {1, 2};
int8_t velocities[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; 

void matrixSend(uint16_t addr, uint16_t data){
    digitalWrite(CS, 0);
    SPI.transfer16(((addr & 0x0f) << 8) | (data & 0xff));
    digitalWrite(CS, 1);
}

void drawField(){
    for (int i = 0; i < 8; i++){
        uint16_t val = 0;
        for(int j = 0; j < 8; j++){
            val |= (field[i][j] << j);
        }
        matrixSend(i+1, val);
        Serial.print(i+1);
        Serial.print(' ');
        Serial.println(val, BIN);
    }
}

void renderField(){
    memset(field, 0, 64);
    for(int i = snake_start; i != snake_end; i++){
        field[snake_coords[i][0]][snake_coords[i][1]] = 1;
    }
    if (millis() % 666 < 333)field[food_coords[0]][food_coords[1]] = 1;

}

void setup()
{
    Serial.begin(115200);
	pinMode(13, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(CS, OUTPUT);
    SPI.begin();
    digitalWrite(CS, 1);

    matrixSend(0x09, 0); // no decode
    matrixSend(0x0C, 1); // no shutdown
    matrixSend(0x0b, 0b111); // no scan-limit
    matrixSend(0x0a, 1); // intensity
    // matrixSend(0x01, 0b10101010);

    for(int i = 0; i < 4; i++){
        pinMode(direction_ports[i], INPUT_PULLUP);
    }

}

unsigned long lastTick = 0;

void gameTick(){
    snake_coords[snake_end][0] = (snake_coords[(snake_end + 63)%64][0] + velocity[0]) % 8;
    snake_coords[snake_end][1] = (snake_coords[(snake_end + 63)%64][1] + velocity[1]) % 8;
    snake_end = (snake_end + 1) % 64;
    snake_start = (snake_start + 1) % 64;

}

void loop()
{
    for(int i = 0; i < 4; i++){
        if (digitalRead(direction_ports[i])){
            velocity[0] = velocities[i][0];
            velocity[1] = velocities[i][1];
        }
    }

    if(lastTick + 500 < millis()){
        lastTick = millis();
        gameTick();
    }

    renderField();
    drawField();
    // matrixSend(0x01, 0b10101010);
    // delay(1000);

}
