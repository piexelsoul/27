Overview
confident.c is a C program that calculates a confidence score for trading strategies based on technical analysis indicators. Using the ncurses library, it provides a terminal-based interface to input market conditions, compute a score, and save results. Ideal for traders quantifying trade setups.
Features

    Inputs: Wave position (Elliott Wave), trend alignment, MACD divergence, Bollinger Bands status, volume support, price behavior, and strategy name.
    Scoring: Assigns points based on inputs (e.g., Wave3 = 25 points, trend = 20/5 points) to compute a confidence percentage.
    Interface: Interactive terminal UI with ncurses for input prompts and result display.
    Output: Displays strategy name and score; saves results to confidence_result.txt on request.
    Looping: Allows multiple runs without restarting.

How It Works

    Enter market conditions (e.g., wave3, trend=true, Bollinger Bands=overbought).
    Program validates inputs and calculates a score based on predefined weights.
    View results on-screen; choose to save, rerun, or exit.

Use Case

    Traders analyzing stocks, forex, or crypto can evaluate trade setups.
    Example: Input a wave5 setup with MACD divergence and volume support to get a confidence score for a trade decision.

Requirements

    ncurses library (libncurses-dev on Linux).
    C compiler (e.g., gcc).

Installation
bash

# Install ncurses (Ubuntu/Debian)
sudo apt-get install libncurses5-dev libncursesw5-dev

# Compile
gcc -o confident confident.c -lncurses

# Run
./confident

Usage

    Run the program in a terminal.
    Follow prompts to input market conditions.
    View confidence score and strategy name.
    Press r to rerun, s to save results, or any key to exit.

Notes

    Ensure ncurses is installed to compile.
    Scoring is simplistic; adjust weights in code for custom strategies.
    Saved results overwrite confidence_result.txt.
Please confirm which document(s) you meant or if this README meets your needs!
