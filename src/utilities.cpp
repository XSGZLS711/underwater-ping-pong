#include "splashkit.h"
#include "splashkit-arrays.h"

void swap(int &n1, int &n2){
    int temp;
    temp = n1;
    n1 = n2;
    n2 = temp;
}

string read_string(string prompt){
    write(prompt);
    return read_line();
}

bool read_boolean(string prompt){
    string input;
    input = read_string(prompt);
    while(!(input == "Y" || input == "y" || input == "N" || input =="n")){
        input = read_string("Please enter (y/n): ");
    }
    return input == "Y" || input == "y";
}

int read_integer(string prompt){
    string input;
    input = read_string(prompt);

    while(!is_integer(input)){
        write_line("Please enter an integer.");
        input = read_string(prompt);
    }

    return to_integer(input);
}

int read_integer(string prompt, int multiple){
    string input;
    input = read_string(prompt);

    while(!is_integer(input) || to_integer(input) % multiple != 0){
        write_line("Please enter a multiple of " + to_string(multiple));
        input = read_string(prompt);
    }

    return to_integer(input);
}

int read_integer(string prompt, int low, int high){
    int input = read_integer(prompt);
    while(input < low || input > high){
        write_line("Please enter a value between " + to_string(low) + " - " + to_string(high) + ".");
        input = read_integer(prompt);
    }
    return input;
}

double read_double(string prompt){
    string input;
    input = read_string(prompt);

    while(!is_double(input)){
        write_line("Please enter a double.");
        input = read_string(prompt);
    }

    return to_double(input);
}

void print_repeated(string text, int times, bool with_line){
    for(int i = 0; i < times; i++){
        write(text);
    }
    if(with_line){
        write_line();
    }
}

void print_line(int length){
    print_repeated("-", length, true);
}