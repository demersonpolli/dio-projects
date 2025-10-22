package com.demersonpolli;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;


public class SudokuGUI extends JFrame {
    private static final int SIZE = 9;
    private static final int CELL_SIZE = 50;
    private JTextField[][] initialCells;
    private JTextField[][] solvedCells;

    // Strings used in the class
    private String INITIAL_BOARD = "Tabuleiro inicial";
    private String SOLVED_BOARD = "Tabuleiro resolvido";
    private String LOAD_PUZZLE = "Carregar jogo";
    private String SOLVE = "Resolver";
    private String SELECT_BOARD = "Selecione o arquivo do tabuleiro Sudoku";
    private String BOARD_LOADED = "Tabuleiro carregado com sucesso!";
    private String SUCCESS = "Sucesso";
    private String ERROR_LOADING_FILE = "Erro ao ler arquivo: ";
    private String ERROR = "Erro";
    private String PUZZLE_SOLVED = "Sudoku resolvido com sucesso!";
    private String NO_SOLUTION_EXISTS = "Nenhuma solução existe para este jogo.";
    private String NO_SOLUTION = "Nenhuma solução encontrada.";
    private String ERROR_SOLVING_PUZZLE = "Erro ao resolver o Sudoku: ";

    public SudokuGUI() {
        setTitle("Sudoku Solver");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout(20, 20));

        // Main panel with padding
        JPanel mainPanel = new JPanel(new BorderLayout(20, 20));
        mainPanel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));

        // Center panel for both boards
        JPanel centerPanel = new JPanel(new GridLayout(1, 2, 20, 0));

        // Initial board panel
        JPanel initialPanel = new JPanel(new BorderLayout());
        JLabel initialLabel = new JLabel(INITIAL_BOARD, SwingConstants.CENTER);
        initialLabel.setFont(new Font("Arial", Font.BOLD, 16));
        initialPanel.add(initialLabel, BorderLayout.NORTH);
        initialCells = createBoardPanel();
        initialPanel.add(initialCells[0][0].getParent().getParent(), BorderLayout.CENTER);

        // Solved board panel
        JPanel solvedPanel = new JPanel(new BorderLayout());
        JLabel solvedLabel = new JLabel(SOLVED_BOARD, SwingConstants.CENTER);
        solvedLabel.setFont(new Font("Arial", Font.BOLD, 16));
        solvedPanel.add(solvedLabel, BorderLayout.NORTH);
        solvedCells = createBoardPanel();
        solvedPanel.add(solvedCells[0][0].getParent().getParent(), BorderLayout.CENTER);

        centerPanel.add(initialPanel);
        centerPanel.add(solvedPanel);

        mainPanel.add(centerPanel, BorderLayout.CENTER);

        // Button panel at bottom
        JPanel buttonPanel = new JPanel(new FlowLayout());
        JButton loadButton = new JButton(LOAD_PUZZLE);
        JButton solveButton = new JButton(SOLVE);

        loadButton.addActionListener(e -> loadPuzzle());
        solveButton.addActionListener(e -> solvePuzzle());

        buttonPanel.add(loadButton);
        buttonPanel.add(solveButton);

        mainPanel.add(buttonPanel, BorderLayout.SOUTH);

        add(mainPanel);
        pack();
        setLocationRelativeTo(null);
    }

    private JTextField[][] createBoardPanel() {
        JPanel boardPanel = new JPanel(new GridLayout(3, 3, 5, 5));
        boardPanel.setBackground(Color.BLACK);
        JTextField[][] cells = new JTextField[SIZE][SIZE];

        for (int blockRow = 0; blockRow < 3; blockRow++) {
            for (int blockCol = 0; blockCol < 3; blockCol++) {
                JPanel subgridPanel = new JPanel(new GridLayout(3, 3, 1, 1));
                subgridPanel.setBackground(Color.GRAY);
                subgridPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK, 2));

                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        int row = blockRow * 3 + i;
                        int col = blockCol * 3 + j;

                        JTextField cell = new JTextField();
                        cell.setHorizontalAlignment(JTextField.CENTER);
                        cell.setFont(new Font("Arial", Font.BOLD, 20));
                        cell.setPreferredSize(new Dimension(CELL_SIZE, CELL_SIZE));
                        cell.setEditable(false);
                        cell.setBackground(Color.WHITE);

                        cells[row][col] = cell;
                        subgridPanel.add(cell);
                    }
                }
                boardPanel.add(subgridPanel);
            }
        }

        return cells;
    }

    private void loadPuzzle() {
        JFileChooser fileChooser = new JFileChooser(".");
        fileChooser.setDialogTitle(SELECT_BOARD);
        int result = fileChooser.showOpenDialog(this);

        if (result == JFileChooser.APPROVE_OPTION) {
            try {
                SudokuSolver solver = new SudokuSolver();
                solver.readFromFile(fileChooser.getSelectedFile().getAbsolutePath());
                int[][] board = solver.getBoard();

                // Display on initial board
                displayBoard(initialCells, board, null);

                // Clear solved board
                clearBoard(solvedCells);

                JOptionPane.showMessageDialog(this, BOARD_LOADED, SUCCESS, JOptionPane.INFORMATION_MESSAGE);
            } catch (IOException ex) {
                JOptionPane.showMessageDialog(this, ERROR_LOADING_FILE + ex.getMessage(),
                    ERROR, JOptionPane.ERROR_MESSAGE);
            } catch (IllegalArgumentException ex) {
                JOptionPane.showMessageDialog(this, ERROR + ": " + ex.getMessage(),
                    ERROR, JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    private void solvePuzzle() {
        try {
            // Get the current initial board
            int[][] initialBoard = getBoardFromCells(initialCells);

            // Create solver with initial board
            SudokuSolver solver = new SudokuSolver();
            solver.setBoard(initialBoard);

            // Get copy of initial board for comparison
            int[][] originalBoard = solver.getBoard();

            // Solve
            if (solver.solve()) {
                int[][] solvedBoard = solver.getBoard();
                displayBoard(solvedCells, solvedBoard, originalBoard);
                JOptionPane.showMessageDialog(this, PUZZLE_SOLVED, SUCCESS, JOptionPane.INFORMATION_MESSAGE);
            } else {
                JOptionPane.showMessageDialog(this, NO_SOLUTION_EXISTS, NO_SOLUTION, JOptionPane.WARNING_MESSAGE);
            }
        } catch (Exception ex) {
            JOptionPane.showMessageDialog(this, ERROR_SOLVING_PUZZLE + ex.getMessage(),
                ERROR, JOptionPane.ERROR_MESSAGE);
        }
    }

    private int[][] getBoardFromCells(JTextField[][] cells) {
        int[][] board = new int[SIZE][SIZE];
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                String text = cells[row][col].getText().trim();
                if (text.isEmpty()) {
                    board[row][col] = 0;
                } else {
                    board[row][col] = Integer.parseInt(text);
                }
            }
        }
        return board;
    }

    private void displayBoard(JTextField[][] cells, int[][] board, int[][] originalBoard) {
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                int value = board[row][col];
                if (value == 0) {
                    cells[row][col].setText("");
                    cells[row][col].setBackground(Color.WHITE);
                } else {
                    cells[row][col].setText(String.valueOf(value));

                    // If originalBoard is provided, color solved cells differently
                    if (originalBoard != null && originalBoard[row][col] == 0) {
                        cells[row][col].setForeground(Color.BLUE);
                    } else {
                        cells[row][col].setForeground(Color.BLACK);
                    }
                    cells[row][col].setBackground(Color.WHITE);
                }
            }
        }
    }

    private void clearBoard(JTextField[][] cells) {
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                cells[row][col].setText("");
                cells[row][col].setForeground(Color.BLACK);
                cells[row][col].setBackground(Color.WHITE);
            }
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                // Set the system look and feel
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception e) {
                // If setting system look and feel fails, continue with default
                System.err.println("Could not set system look and feel: " + e.getMessage());
            }

            SudokuGUI gui = new SudokuGUI();
            gui.setVisible(true);
        });
    }
}
