#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

const int SIZE = 9;
const int EMPTY_CELL = 0;

struct SudokuGame {
    vector<vector<int>> originalBoard;
    vector<vector<int>> board;
    int maxWrongAttempts;
};

bool isSafe(vector<vector<int>>& board, int row, int col, int num) {
    for (int x = 0; x < SIZE; x++) {
        if (board[row][x] == num || board[x][col] == num) {
            return false;
        }
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i + startRow][j + startCol] == num) {
                return false;
            }
        }
    }

    return true;
}

bool solveSudoku(vector<vector<int>>& board) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == EMPTY_CELL) {
                for (int num = 1; num <= SIZE; num++) {
                    if (isSafe(board, row, col, num)) {
                        board[row][col] = num;
                        if (solveSudoku(board)) {
                            return true;
                        }
                        board[row][col] = EMPTY_CELL;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

void printSudoku(vector<vector<int>>& board) {
    cout << "|-----------------------------------|" << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << "|  ";
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == EMPTY_CELL) {
                cout << ".  ";
            } else {
                cout << board[i][j] << "  ";
            }
            if ((j + 1) % 3 == 0) cout << "|  ";
        }
        cout << endl;

        if ((i + 1) % 3 == 0 && i < SIZE - 1) {
            cout << "|-----------|-----------|-----------|" << endl;
        }
    }
    cout << "|-----------------------------------|" << endl;
}

void saveGame(const SudokuGame& game, const string& playerName, const string& mode) {
    ofstream outFile(playerName + "_" + mode + "_game.txt");
    if (outFile.is_open()) {
        outFile << game.maxWrongAttempts << endl;
        for (const auto& row : game.originalBoard) {
            for (int num : row) {
                outFile << num << " ";
            }
            outFile << endl;
        }
        for (const auto& row : game.board) {
            for (int num : row) {
                outFile << num << " ";
            }
            outFile << endl;
        }
        cout << "Game saved successfully!" << endl;
        outFile.close();
    } else {
        cout << "Failed to save the game." << endl;
    }
}

bool loadGame(SudokuGame& game, const string& playerName, const string& mode) {
    ifstream inFile(playerName + "_" + mode + "_game.txt");
    if (inFile.is_open()) {
        inFile >> game.maxWrongAttempts;
        game.originalBoard = vector<vector<int>>(SIZE, vector<int>(SIZE, 0));
        game.board = vector<vector<int>>(SIZE, vector<int>(SIZE, 0));

        for (auto& row : game.originalBoard) {
            for (int& num : row) {
                inFile >> num;
            }
        }

        cout << "Original Sudoku Puzzle:" << endl;
        printSudoku(game.originalBoard);

        for (auto& row : game.board) {
            for (int& num : row) {
                inFile >> num;
            }
        }
        cout << "Continuing from where you left off..." << endl;
        cout << "Current Sudoku Puzzle:" << endl;
        printSudoku(game.board);

        inFile.close();
        return true;
    } else {
        cout << "No saved game found for " << playerName << " in " << mode << " mode." << endl;
        return false;
    }
}

int main() {
    string playerName;
    cout << "Enter your name: ";
    getline(cin, playerName);

    cout << "Welcome, " << playerName << ", to Sudoku Solver!" << endl;

    while (true) {
        cout << "Choose an option:" << endl;
        cout << "1. Play a new game" << endl;
        cout << "2. Solve your own Sudoku" << endl;
        cout << "3. Play a saved game" << endl;
        cout << "4. Quit" << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore(); // Consume newline character

        if (choice == 1) {
            string mode;
            int maxWrongAttempts;
            cout << "Choose a difficulty level:" << endl;
            cout << "1. Easy (5 chances)" << endl;
            cout << "2. Medium (3 chances)" << endl;
            cout << "3. Hard (1 chance)" << endl;
            cout << "Enter your choice: ";
            int difficulty;
            cin >> difficulty;

            if (difficulty == 1) {
                mode = "easy";
                maxWrongAttempts = 5;
            } else if (difficulty == 2) {
                mode = "medium";
                maxWrongAttempts = 3;
            } else if (difficulty == 3) {
                mode = "hard";
                maxWrongAttempts = 1;
            } else {
                cout << "Invalid choice! Starting a new game with Easy mode." << endl;
                mode = "easy";
                maxWrongAttempts = 5;
            }

            SudokuGame game;
            bool hasSavedGame = loadGame(game, playerName, mode);
            bool isNewGame = !hasSavedGame;

            if (isNewGame) {
                vector<vector<int>> sudokuBoard(SIZE, vector<int>(SIZE, 0));
                int numEntriesToRemove = 40;
                srand(time(0));
                solveSudoku(sudokuBoard);

                // Remove random entries to create a puzzle
                while (numEntriesToRemove > 0) {
                    int row = rand() % SIZE;
                    int col = rand() % SIZE;
                    if (sudokuBoard[row][col] != EMPTY_CELL) {
                        sudokuBoard[row][col] = EMPTY_CELL;
                        numEntriesToRemove--;
                    }
                }

                game.originalBoard = sudokuBoard;
                game.board = sudokuBoard;
                cout << "Here's your Sudoku Puzzle:" << endl;
                printSudoku(sudokuBoard);
            }

            int wrongAttempts = 0;
            bool hasWon = false; // Variable to track if the user has won

            while (true) {
                int row, col, num;
                cout
                        << "Enter row (1-9), column (1-9), and number (1-9) separated by spaces (or enter 0 0 0 to quit): ";
                cin >> row >> col >> num;

                // Check if the user wants to quit
                if (row == 0 && col == 0 && num == 0) {
                    char saveChoice;
                    cout << "Do you want to save your progress? (y/n): ";
                    cin >> saveChoice;
                    if (saveChoice == 'y' || saveChoice == 'Y') {
                        saveGame(game, playerName, mode);
                    }
                    cout << "Thanks for playing, " << playerName << "! Goodbye!" << endl;
                    break;
                }

                // Convert user input to 0-based index
                row--;
                col--;

                // Validate user input
                if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && num >= 1 && num <= 9) {
                    if (game.board[row][col] == EMPTY_CELL) {
                        if (isSafe(game.board, row, col, num)) {
                            game.board[row][col] = num;
                            cout << "Move accepted. Updated Sudoku Puzzle:" << endl;
                            printSudoku(game.board);
                        } else {
                            cout << "Invalid move! Number " << num << " cannot be placed at row " << row + 1
                                 << ", column " << col + 1 << "." << endl;
                            wrongAttempts++;
                        }
                    } else {
                        cout << "Invalid move! Cell at row " << row + 1 << ", column " << col + 1
                             << " is already filled." << endl;
                        wrongAttempts++;
                    }
                } else {
                    cout << "Invalid input! Please enter valid row, column, and number (1-9)." << endl;
                    wrongAttempts++;
                }

                // Check if the user has made more than the allowed incorrect attempts
                if (wrongAttempts >= maxWrongAttempts) {
                    cout << "Game Over, " << playerName << "! Better luck next time. Here is the correct solution:"
                         << endl;
                    solveSudoku(game.originalBoard);
                    printSudoku(game.originalBoard);
                    break;
                }

                // Check if the user has filled all entries correctly
                bool allEntriesFilled = true;
                for (int i = 0; i < SIZE; ++i) {
                    for (int j = 0; j < SIZE; ++j) {
                        if (game.board[i][j] == EMPTY_CELL) {
                            allEntriesFilled = false;
                            break;
                        }
                    }
                    if (!allEntriesFilled) {
                        break;
                    }
                }

                if (allEntriesFilled) {
                    hasWon = true;
                    cout << "Congratulations, you won!" << endl;
                    break; // Break out of the inner loop
                }


            }

            // After the inner loop, check if the user has won and continue to the next iteration of the outer loop
            if (hasWon) {
                continue;
            }
        } else if (choice == 2) {
            vector<vector<int>> sudokuBoard(SIZE, vector<int>(SIZE, 0));
            cout << "Enter the Sudoku puzzle cell by cell (use 0 for empty cells)." << endl;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    cout << "Enter value for cell [" << i + 1 << "][" << j + 1 << "]: ";
                    cin >> sudokuBoard[i][j];
                    if (sudokuBoard[i][j] < 0 || sudokuBoard[i][j] > 9) {
                        cout << "Invalid input! Please enter a number between 0 and 9." << endl;
                        j--; // Decrement j to re-enter the value for the same cell
                    }
                }
            }

            cout << "Solving the Sudoku puzzle..." << endl;
            if (solveSudoku(sudokuBoard)) {
                cout << "Sudoku puzzle solved:" << endl;
                printSudoku(sudokuBoard);
            }
        } else if (choice == 3) {
            string mode;
            cout << "Enter the mode (easy/medium/hard): ";
            cin >> mode;
            SudokuGame game;
            bool hasSavedGame = loadGame(game, playerName, mode);
            if (!hasSavedGame) {
                cout << "No saved game found for " << playerName << " in " << mode << " mode." << endl;
                continue;
            }

            int wrongAttempts = 0;
            bool hasWon = false;

            while (true) {
                int row, col, num;
                cout
                        << "Enter row (1-9), column (1-9), and number (1-9) separated by spaces (or enter 0 0 0 to quit): ";
                cin >> row >> col >> num;

                if (row == 0 && col == 0 && num == 0) {
                    char saveChoice;
                    cout << "Do you want to save your progress? (y/n): ";
                    cin >> saveChoice;
                    if (saveChoice == 'y' || saveChoice == 'Y') {
                        saveGame(game, playerName, mode);
                    }
                    cout << "Thanks for playing, " << playerName << "! Goodbye!" << endl;
                    break;
                }

                row--;
                col--;

                if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && num >= 1 && num <= 9) {
                    if (game.board[row][col] == EMPTY_CELL) {
                        if (isSafe(game.board, row, col, num)) {
                            game.board[row][col] = num;
                            cout << "Move accepted. Updated Sudoku Puzzle:" << endl;
                            printSudoku(game.board);
                        } else {
                            cout << "Invalid move! Number " << num << " cannot be placed at row " << row + 1
                                 << ", column " << col + 1 << "." << endl;
                            wrongAttempts++;
                        }
                    } else {
                        cout << "Invalid move! Cell at row " << row + 1 << ", column " << col + 1
                             << " is already filled." << endl;
                        wrongAttempts++;
                    }
                } else {
                    cout << "Invalid input! Please enter valid row, column, and number (1-9)." << endl;
                    wrongAttempts++;
                }

                if (wrongAttempts >= game.maxWrongAttempts) {
                    cout << "Game Over, " << playerName << "! Better luck next time. Here is the correct solution:"
                         << endl;
                    solveSudoku(game.originalBoard);
                    printSudoku(game.originalBoard);
                    break;
                }

                bool allEntriesFilled = true;
                for (int i = 0; i < SIZE; ++i) {
                    for (int j = 0; j < SIZE; ++j) {
                        if (game.board[i][j] == EMPTY_CELL) {
                            allEntriesFilled = false;
                            break;
                        }
                    }
                    if (!allEntriesFilled) {
                        break;
                    }
                }

                if (allEntriesFilled) {
                    hasWon = true;
                    cout << "Congratulations, you won!" << endl;
                    break;
                }
            }
        } else if (choice == 4) {
            cout << "Thanks for playing, " << playerName << "! Goodbye!" << endl;
            break;
        } else {
            cout << "Invalid choice! Please enter 1, 2, 3, or 4." << endl;
        }


        return 0;
    }
}
