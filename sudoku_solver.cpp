#include<iostream>
#include<string>
#include<array>
#include<stdlib.h>
#include<algorithm>
#include<math.h>
#include<chrono>
#include<random>
#include<time.h>
#include<thread>
#include<Windows.h>

using namespace std;

void sleep(int milliseconds){
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

const int PUZZLE_SIZE = 9;
const int EMPTY_VALUE = 0;
std::array<int, 9> values{1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9};
int numberOfSolution = 0;

void setCursorPosition(int x, int y){
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hOut, coord);
}

void printPuzzle(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE], bool clear = true){

    if(clear){
        setCursorPosition(0, 0);
    }

    string text = "|";
    string separator = " -";
    string padding = "|";
    for(int i=0;i<PUZZLE_SIZE;i++){
        text = "|";
        separator = " -";
        padding = "|";
        for(int j=0;j<PUZZLE_SIZE;j++){
            string value = puzzle[i][j] == EMPTY_VALUE ? " " : to_string(puzzle[i][j]);
            text += "  " + value + "  |";
            separator += "------";
            padding += "     |";

            if(j%3 == 2 && j != PUZZLE_SIZE-1){
                padding += "|";
                text += "|";
            }
        }

        if(i%3 == 0 && i != 0){
            replace(separator.begin(),separator.end(), '-','=');
        }

        std::cout<<separator<<endl;
        std::cout<<padding<<endl;
        std::cout<<text<<endl;
        std::cout<<padding<<endl;
    }
    std::cout<<separator<<endl;
}

bool isValid(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE] ,int row ,int col ,int value){
    
    // check if this row has already this value
    for(int c=0; c < PUZZLE_SIZE; c++){
        if(puzzle[row][c] == value) return false;
    }

    // check if this column has already this value
    for(int r = 0; r < PUZZLE_SIZE; r++){
        if(puzzle[r][col] == value) return false;
    }

    int startRow = floor(row/3)*3 ,startCol = floor(col/3)*3;
    for(int r = startRow; r < startRow + 3; r++){
        for(int c = startCol; c < startCol + 3; c++){
            if(puzzle[r][c] == value) return false;
        }
    }

    return true;
}

bool hasEmptyCell(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE]){
    for(int i=0; i < PUZZLE_SIZE; i++){
        for(int j=0; j < PUZZLE_SIZE; j++){
            if(puzzle[i][j] == EMPTY_VALUE) return true;
        }
    }
    return false;
}

bool fillPuzzle(int (&puzzle)[PUZZLE_SIZE][PUZZLE_SIZE]){
    int row, col;

    for(int i=0;i < PUZZLE_SIZE*PUZZLE_SIZE;i++){
        row = floor(i/PUZZLE_SIZE);
        col = i%PUZZLE_SIZE;

        if(puzzle[row][col] == EMPTY_VALUE){
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(values.begin(), values.end() ,std::default_random_engine(seed));

            for(int j=0; j < PUZZLE_SIZE; j++){
                if(isValid(puzzle, row, col, values[j])){
                    puzzle[row][col] = values[j];

                    if(!hasEmptyCell(puzzle) || fillPuzzle(puzzle)){
                        return true;
                    }
                }
            }

            break;
        }
    }

    puzzle[row][col] = EMPTY_VALUE;
    return false;
}

bool solveSudoku(int (&puzzle)[PUZZLE_SIZE][PUZZLE_SIZE], bool visualize = false){
    int row,col;

    for(int i=0; i < PUZZLE_SIZE*PUZZLE_SIZE; i++){
        row = floor(i/PUZZLE_SIZE);
        col = i%PUZZLE_SIZE;

        if(puzzle[row][col] == EMPTY_VALUE){
            for(int value = 0; value <= PUZZLE_SIZE; value++){
                if(isValid(puzzle, row, col, value)){
                    puzzle[row][col] = value;

                    if(visualize){
                        sleep(100);
                        printPuzzle(puzzle);
                    }

                    if(!hasEmptyCell(puzzle)){
                        numberOfSolution++;
                        if(visualize){
                        sleep(100);
                        printPuzzle(puzzle);
                        return true;
                    }
                        break;
                    }
                    else if(solveSudoku(puzzle, visualize)){
                        return true;
                    }
                }
            }

            break;
        }
    }

    puzzle[row][col] = EMPTY_VALUE;

    if(visualize){
        sleep(100);
        printPuzzle(puzzle);
    }

    return false;
}

void generatePuzzle(int (&puzzle)[PUZZLE_SIZE][PUZZLE_SIZE], int difficulty = 1){
    for(int i=0;i<PUZZLE_SIZE;i++){
        for(int j=0;j<PUZZLE_SIZE;j++){
            puzzle[i][j] = EMPTY_VALUE;
        }
    }

    fillPuzzle(puzzle);

    // start remove cells from a fullfilled sudoku puzzle

    int attempt = difficulty;
    srand((unsigned)time(0));

    while(attempt > 0){
        // generate random values between 0 to 8
        int row = floor(rand()%PUZZLE_SIZE);
        int col = floor(rand()%PUZZLE_SIZE);

        while(puzzle[row][col] == EMPTY_VALUE){
            row = floor(rand()%PUZZLE_SIZE);
            col = floor(rand()%PUZZLE_SIZE);
        }

        int oldValue = puzzle[row][col];
        puzzle[row][col] = EMPTY_VALUE;

        numberOfSolution = 0;
        solveSudoku(puzzle);


        // if by setting this cell to empty, the sudoku puzzle will not have only one solution
        // then we will not remove this cell
        if(numberOfSolution != 1){
            puzzle[row][col] = oldValue;
            attempt--;
        }
    }
}

int main(){
    int puzzle[PUZZLE_SIZE][PUZZLE_SIZE];

    generatePuzzle(puzzle);
    printPuzzle(puzzle, true);

    std::string solve;
    std::cout<<"Do you want to run algorithm to solve this puzzle (y/n) ";
    std::cin>>solve;

    if(solve == "n" || solve == "N"){
        std::cout.flush();
        return 0;
    }

    solveSudoku(puzzle, true);
    std::cout.flush();
    
    return 0;
}