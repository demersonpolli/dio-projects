package com.demersonpolli;

import java.io.*;

public class SudokuSolver {
    private static final int SIZE = 9;
    private static final int SUBGRID_SIZE = 3;
    private int[][] board;

    public SudokuSolver() {
        board = new int[SIZE][SIZE];
    }

    // Read board from file
    public void readFromFile(String filename) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(filename));
        String line;
        int row = 0;

        while ((line = reader.readLine()) != null && row < SIZE) {
            if (line.length() < SIZE) {
                reader.close();
                throw new IllegalArgumentException("Invalid line length at row " + row);
            }

            for (int col = 0; col < SIZE; col++) {
                char c = line.charAt(col);
                if (c == '#') {
                    board[row][col] = 0;
                } else if (c >= '1' && c <= '9') {
                    board[row][col] = c - '0';
                } else {
                    reader.close();
                    throw new IllegalArgumentException("Invalid character at row " + row + ", col " + col);
                }
            }
            row++;
        }

        reader.close();

        if (row < SIZE) {
            throw new IllegalArgumentException("Not enough rows in file");
        }
    }

    // Check if placing num at board[row][col] is valid
    private boolean isValid(int row, int col, int num) {
        // Check row
        for (int c = 0; c < SIZE; c++) {
            if (board[row][c] == num) {
                return false;
            }
        }

        // Check column
        for (int r = 0; r < SIZE; r++) {
            if (board[r][col] == num) {
                return false;
            }
        }

        // Check 3x3 subgrid
        int startRow = (row / SUBGRID_SIZE) * SUBGRID_SIZE;
        int startCol = (col / SUBGRID_SIZE) * SUBGRID_SIZE;
        for (int r = startRow; r < startRow + SUBGRID_SIZE; r++) {
            for (int c = startCol; c < startCol + SUBGRID_SIZE; c++) {
                if (board[r][c] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    // Solve using backtracking
    public boolean solve() {
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (board[row][col] == 0) {
                    for (int num = 1; num <= 9; num++) {
                        if (isValid(row, col, num)) {
                            board[row][col] = num;

                            if (solve()) {
                                return true;
                            }

                            // Backtrack
                            board[row][col] = 0;
                        }
                    }
                    return false;
                }
            }
        }
        return true;
    }

    // Get a copy of the board
    public int[][] getBoard() {
        int[][] copy = new int[SIZE][SIZE];
        for (int i = 0; i < SIZE; i++) {
            System.arraycopy(board[i], 0, copy[i], 0, SIZE);
        }
        return copy;
    }

    // Set the board
    public void setBoard(int[][] newBoard) {
        for (int i = 0; i < SIZE; i++) {
            System.arraycopy(newBoard[i], 0, board[i], 0, SIZE);
        }
    }

    // Print the board
    public void printBoard() {
        for (int row = 0; row < SIZE; row++) {
            if (row % 3 == 0 && row != 0) {
                System.out.println("------+-------+------");
            }
            for (int col = 0; col < SIZE; col++) {
                if (col % 3 == 0 && col != 0) {
                    System.out.print("| ");
                }
                System.out.print(board[row][col] + " ");
            }
            System.out.println();
        }
    }
}

