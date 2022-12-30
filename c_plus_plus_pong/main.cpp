#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <tuple>

using namespace std;

#define WINDOW_SIZE_X 1500
#define WINDOW_SIZE_Y 500

#define Y_LIM_SLIDER_UP 5
#define Y_LIM_SLIDER_DOWN 445
#define X_LIM_BALL 5
#define Y_LIM_BALL 5

#define BALL_SIZE 10.0

#define MAX_SPEED_SLIDER 5.0
#define DELTA_SPEED_SLIDER 1.0

#define MAX_SPEED_BALL 5.0
#define DELTA_SPEED_BALL 1.0

bool pause_game = true;
bool stream = true;
bool slider_velocity=false;

bool left_slider_manual = false;
bool right_slider_manual = false;
bool one_person_playing = true;

float delay = 0.5;

void set_stream_setting(){
    if ((left_slider_manual) | (right_slider_manual)){
        stream = true;
        one_person_playing = true;
        delay = 0.5;
        if ((left_slider_manual) & (right_slider_manual)){
            stream = false;
        }
    }
    else{
        stream = true;
        one_person_playing = false;
        delay = 0.01;
    }
    std::cout << "Stream: " << stream << " Left Manual: " << left_slider_manual << " Right Manual: ";
    std::cout << right_slider_manual << std::endl;
}


// Font source https://www.fontspace.com/get/family/1137
class Object
{
public:
    sf::Keyboard::Key key1, key2;
    string name, key1_text, key2_text;
    Object(string name, sf::Keyboard::Key key1, string key_1_text, sf::Keyboard::Key key2, string key_2_text){
        this->name = name;
        this->key1 = key1; this->key1_text = key_1_text;
        this->key2 = key2; this->key2_text = key_2_text;
    }

    void set_key_1(sf::Keyboard::Key key1, string text){
        this->key1 = key1;
        this->key1_text = text;
    }

    void set_key_2(sf::Keyboard::Key key2, string text){
        this->key2 = key2;
        this->key2_text = text;
    }

    virtual void key_1_pressed(sf::Event *event){

    }

    virtual void key_2_pressed(sf::Event *event){

    }

    void key_pressed(sf::Event *event){
        if (event->key.code == this->key1){
            this->key_1_pressed(event);
        }
        else if (event->key.code == this->key2){
            this->key_2_pressed(event);
        }
    }
};

class RectangleSliders: public Object, public sf::RectangleShape
{

public:
    float velocity;
    sf::Vector2f initial_position;
    int score;
    int reward;
    RectangleSliders(string name, sf::Keyboard::Key key1, string key_1_text, sf::Keyboard::Key key2,
                     string key_2_text, sf::Vector2f size, sf::Vector2f position, sf::Color color):
            Object(name, key1, key_1_text, key2, key_2_text),
            sf::RectangleShape(size){
        this->setFillColor(color);
        this->setPosition(position);
        this->initial_position = position;
        this->velocity = 0;
        this->score = 0;
        this->reward = 0;
    }
    float get_y(){
        return this->getPosition().y;
    }
    void edge_check(){
        float y = this->getPosition().y;
        if (y <= Y_LIM_SLIDER_UP){
            this->velocity=0.f;
            this->move(0.f, 1.f);
        }
        if (y >= Y_LIM_SLIDER_DOWN){
            this->velocity=0.f;
            this->move(0.f, -1.f);
        }
    }

    void update_position(){
        if (!pause_game){
            this->edge_check();
            this->move(0.f, this->velocity);
            if (!slider_velocity){
                this->velocity = 0;
            }
        }
    }
    void increment_velocity(int direction){
        if (direction > 0){
            if (this->velocity < MAX_SPEED_SLIDER){
                this->velocity += DELTA_SPEED_SLIDER;
            }
        }
        else if (direction < 0){
            if (this->velocity > -MAX_SPEED_SLIDER){
                this->velocity -= DELTA_SPEED_SLIDER;
            }
        }
    }
    void key_1_pressed(sf::Event *event) override{
        this->increment_velocity(-1);
    }

    void key_2_pressed(sf::Event *event) override{
        this->increment_velocity(+1);
    }

    void increment_score(){
        this->score++;
    }

    void reset_score(){
        this->score = 0;
    }

    void reset(){
        std::cout << this->name << " Score: " << this->score << "\n";
        this->setPosition(this->initial_position);
        this->velocity = 0;
    }

    string get_string_score(){
        return to_string(this->score);
    }
};

class Ball: public Object, public sf::CircleShape{
public:
    sf::Vector2f velocity, initial_velocity, initial_position;
    sf::Keyboard::Key key3, key4;
    string key3_text, key4_text;

    Ball(string name, sf::Keyboard::Key key1, string key1_text, sf::Keyboard::Key key2, string key2_text,
         sf::Keyboard::Key key3, string key3_text, sf::Keyboard::Key key4, string key4_text,
         float size, sf::Color color, float speed_x, float speed_y):
            Object(name, key1, key1_text, key2, key2_text), sf::CircleShape(size){
        this->setPosition(WINDOW_SIZE_X / 2, WINDOW_SIZE_Y / 2);
        this->velocity = sf::Vector2f(speed_x, speed_y);
        this->initial_velocity = this->velocity;
        this->initial_position = sf::Vector2f(WINDOW_SIZE_X / 2, WINDOW_SIZE_Y / 2);
        this->setFillColor(color);
        this->key3 = key3;
        this->key3_text = key3_text;
        this->key4 = key4;
        this->key4_text = key4_text;
    }

    int update_position(){
        if (!pause_game){
            int score = this->edge_check();
            this->move(this->velocity);
            return score;
        }
        return 0;
    }
    float get_x(){
        return this->getPosition().x;
    }
    float get_y(){
        return this->getPosition().y;
    }
    float get_xdot(){
        return this->velocity.x;
    }
    float get_ydot(){
        return this->velocity.y;
    }
    void flip_x_velocity(){
        this->velocity.x = this->velocity.x *-1;
    }

    void flip_y_velocity(){
        this->velocity.y = this->velocity.y *-1;
    }

    int edge_check(){
        sf::Vector2f current_position = this->getPosition();
        if (current_position.x <= X_LIM_BALL){ // Has hit the left edge, player on right gets one point
            this->flip_x_velocity();
            this->setPosition(X_LIM_BALL + 1, current_position.y);
            return 1;
        }
        if (current_position.x >= WINDOW_SIZE_X - X_LIM_BALL){ // Has hit the right edge, player on left gets one point
            this->flip_x_velocity();
            this->setPosition(WINDOW_SIZE_X - X_LIM_BALL - 1, current_position.y);
            return -1;
        }
        if (current_position.y <= Y_LIM_BALL){
            this->flip_y_velocity();
            this->setPosition(current_position.x, Y_LIM_BALL + 1);
            this->increment_speed_y(+1.f);
        }
        if (current_position.y >= WINDOW_SIZE_Y - Y_LIM_BALL){
            this->flip_y_velocity();
            this->setPosition(current_position.x, WINDOW_SIZE_Y - Y_LIM_BALL - 1);
            this->increment_speed_y(-1.f);
        }
        return 0;
    }

    float increment_speed(float current_speed, float delta){
        float new_velocity = current_speed + delta;
        if (new_velocity > 0){
            if (new_velocity > MAX_SPEED_BALL){
                return MAX_SPEED_BALL;
            }
            else{
                return new_velocity;
            }
        }
        else {
            if (new_velocity < -MAX_SPEED_BALL) {
                return -MAX_SPEED_BALL;
            } else {
                return new_velocity;
            }
        }
    }

    void increment_speed_x(float delta_x){
        float current_velocity = this->velocity.x;
        float new_velocity = this->increment_speed(current_velocity, delta_x);
        this->velocity.x = new_velocity;
    }

    void increment_speed_y(float delta_y){
        float current_velocity = this->velocity.y;
        float new_velocity = this->increment_speed(current_velocity, delta_y);
        this->velocity.y = new_velocity;
    }

    void print_velocity(){
        std::cout << "Velocity, x: " << this->velocity.x << " y: " << this->velocity.y << "\n";
    }

    void key_1_pressed(sf::Event *event) override{
        this->increment_speed_x(-DELTA_SPEED_BALL);
        this->print_velocity();
    }

    void key_2_pressed(sf::Event *event) override{
        this->increment_speed_x(DELTA_SPEED_BALL);
        this->print_velocity();
    }

    void key_3_pressed(sf::Event *event){
        this->increment_speed_y(-DELTA_SPEED_BALL);
        this->print_velocity();
    }

    void key_4_pressed(sf::Event *event){
        this->increment_speed_y(DELTA_SPEED_BALL);
        this->print_velocity();
    }

    void key_pressed(sf::Event *event){
        if (event->key.code == this->key1){
            this->key_1_pressed(event);
        }
        else if (event->key.code == this->key2){
            this->key_2_pressed(event);
        }
        else if (event->key.code == this->key3){
            this->key_3_pressed(event);
        }
        else if (event->key.code == this->key4){
            this->key_4_pressed(event);
        }
    }

    void reset(int score){
        this->velocity = this->initial_velocity;
        this->setPosition(this->initial_position);
        this->velocity.x *= score;
        this->velocity.y *= score;
    }
};



class PauseThing: public Object{
public:
    PauseThing(string name, sf::Keyboard::Key key1, string key_1_text, sf::Keyboard::Key key2,
               string key_2_text):
            Object(name, key1, key_1_text, key2, key_2_text){
    }
    void pause_everything(){
        pause_game = true;
    }
    void resume_everything(){
        pause_game = false;
    }

    void key_1_pressed(sf::Event *event) override{
        this->resume_everything();
    }
    void key_2_pressed(sf::Event *event) override{
        this->pause_everything();
    }

};

class IntegerStream: public sf::TcpSocket{
public:
    sf::IpAddress ip;
    unsigned short port;
    IntegerStream(sf::IpAddress ip, unsigned short port): sf::TcpSocket(){
        this->ip = ip;
        this->port = port;
    }

    void connect_stream(){
        this->connect(this->ip, this->port);
    }

    string convert_input_to_output(float slider1_x, float slider1_xdot, float slider2_x, float slider2_xdot,
                                   float ball_x, float ball_xdot, float ball_y, float ball_ydot,
                                   float reward_slider1, float reward_slider2){
        string output_string = "";
        output_string += to_string(slider1_x) + " ";
        output_string += to_string(slider1_xdot) + " ";
        output_string += to_string(slider2_x) + " ";
        output_string += to_string(slider2_xdot) + " ";
        output_string += to_string(ball_x) + " ";
        output_string += to_string(ball_xdot) + " ";
        output_string += to_string(ball_y) + " ";
        output_string += to_string(ball_ydot) + " ";
        output_string += to_string(reward_slider1) + " ";
        output_string += to_string(reward_slider2) + " ";
        return output_string;
    }

    void convert_char_output_to_int(int* number_pointer, char* sign, char* number){
        *number_pointer = atoi(number);
        if (*sign == '-'){
            *number_pointer *= -1;
        }
    }

    void receive_message(int* pointer1, int* pointer2){
        const static int receive_message_length = 4;
        char buffer[receive_message_length];
        std::size_t received = 0;
        this->receive(buffer, sizeof(buffer), received);
        this->convert_char_output_to_int(pointer1, &buffer[0], &buffer[1]);
        this->convert_char_output_to_int(pointer2, &buffer[2], &buffer[3]);
    }
    void send_and_receive_message(string output_message, int* pointer1, int* pointer2){
        if (this->send(output_message.c_str(), output_message.size()) != sf::Socket::Done){
            std::cout << "Error";
        }
        else{
            this->receive_message(pointer1, pointer2);
        }
    }

    string generate_output_message(RectangleSliders* slider_1_pointer, RectangleSliders* slider_2_pointer,
                                   Ball* ball_pointer){
        RectangleSliders slider1 = *slider_1_pointer;
        RectangleSliders slider2 = *slider_2_pointer;
        Ball ball = *ball_pointer;
        string output_message = this->convert_input_to_output(slider1.get_y() + 25.f,
                                                       slider1.velocity,
                                                       slider2.get_y() + 25.f,
                                                       slider2.velocity,
                                                       ball.get_x(),
                                                       ball.get_xdot(),
                                                       ball.get_y(),
                                                       ball.get_ydot(),
                                                       slider1.reward,
                                                       slider2.reward);
        return output_message;
    }

    void board_commands(RectangleSliders* slider_1_pointer, RectangleSliders* slider_2_pointer,
                        Ball* ball_pointer, bool left_slider_manual, bool right_slider_manual){
        string output_message = this->generate_output_message(slider_1_pointer, slider_2_pointer, ball_pointer);
        int command1, command2;
        this->send_and_receive_message(output_message, &command1, &command2);
        if (!left_slider_manual){
            slider_1_pointer->increment_velocity(command1);
        }
        if (!right_slider_manual){
            slider_2_pointer->increment_velocity(command2);
        }
    }

};

class Playground{
public:
    RectangleSliders* slider1_pointer;
    RectangleSliders* slider2_pointer;
    Ball* ball_pointer;
    string game_name;
    sf::Text* score1_pointer;
    sf::Text* score2_pointer;
    PauseThing* pausething_pointer;
    Playground(string game_name){
        this->game_name = game_name;
    }
    void get_sliders(RectangleSliders* slider1_pointer, RectangleSliders* slider2_pointer){
        this->slider1_pointer = slider1_pointer;
        this->slider2_pointer = slider2_pointer;
    }
    void get_ball(Ball* ball_pointer){
        this->ball_pointer = ball_pointer;
    }

    void get_scores_board(sf::Text* score1_pointer, sf::Text* score2_pointer){
        this->score1_pointer = score1_pointer;
        this->score2_pointer = score2_pointer;
    }

    void get_pause_board(PauseThing* pausething_pointer){
        this->pausething_pointer = pausething_pointer;
    }

    void handle_score(){
        int score = this->ball_pointer->update_position();
        if (score != 0){
            if (score == 1){
                this->slider2_pointer->increment_score();
                this->score2_pointer->setString(this->slider2_pointer->get_string_score());
            }
            else{ // slider 1 scored
                this->slider1_pointer->increment_score();
                this->score1_pointer->setString(this->slider1_pointer->get_string_score());
            }
            this->slider1_pointer->reset();
            this->slider2_pointer->reset();
            this->ball_pointer->reset(score);
            this->pausething_pointer->pause_everything();
        }
    }

    void update_positions(){
        this->handle_score();
        this->slider1_pointer->update_position();
        this->slider2_pointer->update_position();
    }

};

void update_board(sf::Text* text_pointer, sf::Color color,
                  float factor_x, float factor_y, sf::Text::Style style=sf::Text::Bold){
    text_pointer->setFillColor(color);
    text_pointer->setPosition(factor_x * WINDOW_SIZE_X, factor_y * WINDOW_SIZE_Y);
    text_pointer->setStyle(style);
}

int main(){
    set_stream_setting();
    IntegerStream stream_device(sf::IpAddress("localhost"), 55001);
    stream_device.connect_stream();
    int command1, command2;
    sf::Font font;
    font.loadFromFile("SwanseaBold-D0ox.ttf");
    sf::Text score1("0", font, 30);
    sf::Text score2("0", font, 30);
    sf::Text pauseboard("Press 'P' to pause the game and 'C' to resume", font, 30);
    update_board(&score1,  sf::Color::Red, 0.25, 0.15);
    update_board(&score2, sf::Color::Blue, 0.75, 0.15);
    update_board(&pauseboard, sf::Color::Green, 0.25, 0.05);
    PauseThing pause_board("Pause Board", sf::Keyboard::C, "C", sf::Keyboard::P, "P");
    RectangleSliders slider1("Left Slider", sf::Keyboard::W, "W", sf::Keyboard::S, "S", sf::Vector2f(10, 50),
                             sf::Vector2f(0.05 * WINDOW_SIZE_X, 0.5 * WINDOW_SIZE_Y), sf::Color::Red);
    RectangleSliders slider2("Right Slider", sf::Keyboard::Up, "Up", sf::Keyboard::Down, "Down", sf::Vector2f(10, 50),
                             sf::Vector2f(0.95 * WINDOW_SIZE_X, 0.5 * WINDOW_SIZE_Y), sf::Color::Blue);
    Ball ball("4/6", sf::Keyboard::Numpad4, "4", sf::Keyboard::Numpad6, "6", sf::Keyboard::Numpad8, "8",
              sf::Keyboard::Numpad2, "2", BALL_SIZE, sf::Color::Green, MAX_SPEED_BALL, 1.f);
    Playground playground("A Fun Game");
    playground.get_sliders(&slider1, &slider2);
    playground.get_ball(&ball);
    playground.get_scores_board(&score1, &score2);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), playground.game_name);
    while (window.isOpen()){
        if ((stream) & (!one_person_playing)){
            pause_game = false;
        }
        sf::Event event;
        while (window.pollEvent(event)) {
            if ((event.type == sf::Event::KeyPressed)) {
                if (!pause_game){
                    if (left_slider_manual){
                        slider1.key_pressed(&event);
                    }
                    if (right_slider_manual){
                        slider2.key_pressed(&event);
                    }
                    ball.key_pressed(&event);
                }
                pause_board.key_pressed(&event);
            }
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        if (stream){
            stream_device.board_commands(&slider1, &slider2, &ball, left_slider_manual, right_slider_manual);

        }
        playground.update_positions();
        if (ball.getGlobalBounds().intersects(slider1.getGlobalBounds())){
            ball.flip_x_velocity();
            ball.move(5.f, 0.f);
            ball.increment_speed_y(slider1.velocity);
        }
        if (ball.getGlobalBounds().intersects(slider2.getGlobalBounds())){
            ball.flip_x_velocity();
            ball.move(-5.f, 0.f);
            ball.increment_speed_y(slider1.velocity);
        }
        window.clear();
        window.draw(slider1);
        window.draw(slider2);
        window.draw(ball);
        window.draw(score1);
        window.draw(score2);
        window.draw(pauseboard);
        window.display();
        sleep(delay);
    }
    return 0;
}
/* Instructions to run this file
 * g++ -c main.cpp
g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
bash ./sfml-app
 */