package com.demersonpolli;

import java.io.*;

public class SudokuCLI {
    public static void main(String[] args) {
        String filename = "puzzle.txt";
        if (args.length < 1) {
            System.out.println("Usage: java SudokuCLI <filename>");
            System.out.println("Using the default file 'puzzle.txt'");
        }
        else filename = args[0];

        SudokuSolver solver = new SudokuSolver();

        try {
            solver.readFromFile(filename);
            System.out.println("Original Sudoku Board:");
            solver.printBoard();
            System.out.println();

            if (solver.solve()) {
                System.out.println("Solved Sudoku Board:");
                solver.printBoard();
            } else {
                System.out.println("No solution exists for this Sudoku puzzle.");
            }
        } catch (IOException e) {
            System.err.println("Error reading file: " + e.getMessage());
        } catch (IllegalArgumentException e) {
            System.err.println("Error: " + e.getMessage());
        }
    }
}
