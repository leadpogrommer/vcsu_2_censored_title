#include <SPI.h>
#include <stdint.h>

#include "music.h"

#define CS 10
#define SPEAKER A5

int8_t snake_coords [64][2] = {{4, 5}};
uint8_t snake_start = 0;
uint8_t snake_end = 1;
uint8_t field[8][8];

uint8_t direction_ports[] = {2, 5, 3, 4};
int8_t velocity[] = {1, 0};
int8_t food_coords[] = {1, 2};
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
        // Serial.print(i+1);
        // Serial.print(' ');
        // Serial.println(val, BIN);
    }
}

int player_this_note = 0;
int player_total_notes = 0;
const int* player_current_arr;
unsigned long player_delay_start = 0;
unsigned long player_delay_duration = 0;


void player_update(){
    if (player_this_note >= (player_total_notes*2+1)){
        // noTone(SPEAKER);
        return;
    }
    if(player_delay_start + player_delay_duration > millis()) return;
    tone(SPEAKER, player_current_arr[player_this_note], (1000/player_current_arr[player_this_note + 1]));
    player_delay_start = millis();
    player_delay_duration = (1000/player_current_arr[player_this_note + 1] ) * 1.3;
    player_this_note += 2;
}

void player_play(const int *arr){
    player_this_note = 1;
    player_total_notes = arr[0];
    player_current_arr = arr;
    player_delay_start = 0;
    player_delay_duration = 0;
}


uint8_t gameOverText[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111110,
    0b01001000,
    0b01000000,
    0b00000000,
    0b00111110,
    0b01001000,
    0b00111110,
    0b00000000,
    0b01000010,
    0b01111110,
    0b01000010,
    0b00000000,
    0b01111110,
    0b00000010,
    0b00000010,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};
void gameOver(){
    player_play(music_death);
    for(int i = 0; i < (sizeof(gameOverText)) - 8; i++){
        for(int j = 0; j < 8; j++){
            matrixSend(8-j, ~gameOverText[i+j]);
        }
        unsigned long ms = millis();
        while (ms + 500 > millis()){
            player_update();
        }
        // delay(500);
    }
    // delay(1000);
    ((void (*)())0)();
}

void renderField(){
    memset(field, 0, 64);
    for(int i = snake_start; i != snake_end; i = (i+1)%64){
        if(field[snake_coords[i][0]][snake_coords[i][1]]){
            gameOver();
        }
        field[snake_coords[i][0]][snake_coords[i][1]] = 1;
    }
    if (millis() % 300 < 150)field[food_coords[0]][food_coords[1]] = 1;

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
    player_play(music_theme);

}

unsigned long lastTick = 0;
int difficulty = 1;

void gameTick(){

    Serial.println(String() + snake_start + ' ' + snake_end);
    Serial.println(String() + snake_coords[snake_start][0] + ' ' + snake_coords[snake_start][1]); 
    snake_coords[snake_end][0] = (snake_coords[(snake_end + 63)%64][0] + velocity[0]) & 7;
    snake_coords[snake_end][1] = (snake_coords[(snake_end + 63)%64][1] + velocity[1]) & 7;
    Serial.println(String() + snake_coords[snake_end][0] + ' ' + snake_coords[snake_end][1]);
    Serial.println("------------------");
    if (snake_coords[snake_end][0] != food_coords[0] || snake_coords[snake_end][1] != food_coords[1]){
        snake_start = (snake_start + 1) % 64;
    } else {
        bool isOk;
        do {
            food_coords[0] = random(8);
            food_coords[1] = random(8);
            isOk = true;
            for(int i = snake_start; i != (snake_end + 1) % 64; i = (i+1)%64){
                if (food_coords[0] == snake_coords[i][0] && food_coords[1] == snake_coords[i][1]){
                    isOk = false;
                    break;
                }
            }
        }while(!isOk);
        difficulty++;
        player_play(music_life);
        
    }
    snake_end = (snake_end + 1) % 64;

}

void loop()
{
    player_update();
    for(int i = 0; i < 4; i++){
        auto val = digitalRead(direction_ports[i]);
        // Serial.print(val, BIN);
        
        if (!val){
            velocity[0] = velocities[i][0];
            velocity[1] = velocities[i][1];
        }
    }
    // Serial.println();

    if(lastTick + (512 - (difficulty*7)) < millis()){
        lastTick = millis();
        gameTick();
    }

    renderField();
    drawField();
    // matrixSend(0x01, 0b10101010);
    // delay(1000);

}
