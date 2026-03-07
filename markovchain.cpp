#include <filesystem>
#include <fstream>
#include <iostream>

// USING DECLARATIONS
using namespace std;

// VARIABLE DECLARATIONS
// Declare and initialise constants which control game parameters.
const short NUM_BLACK_CARDS = 26;
const short NUM_RED_CARDS = 26;
const short MAX_STREAK = 52;

// Array which stores results in order for file output.
double results[MAX_STREAK];

// FUNCTION DECLARATION
int getIndex(short B, short R, short S, short streakToWin);
void fillMemoryTable(double *memoryTable, short streakToWin);
double P(short B, short R, short S, double *memoryTable, short streakToWin);
void writeToFile();

// MAIN FUNCTION
int main()
{
    for (size_t streakToWin = 1; streakToWin <= MAX_STREAK; streakToWin++)
    {
        const short dimB = NUM_BLACK_CARDS + 1;
        const short dimR = NUM_RED_CARDS + 1;
        const short dimS = streakToWin + 1;

        double *memoryTable = new double[dimB * dimR * dimS];
        fillMemoryTable(memoryTable, streakToWin);

        double startingWinChance = P(NUM_BLACK_CARDS, NUM_RED_CARDS, 0, memoryTable, streakToWin);
        cout << fixed << setprecision(13) << "Probability of Achieving a Streak of "
             << streakToWin << ": " << (startingWinChance * 100) << "%\n";

        results[streakToWin - 1] = startingWinChance;

        delete[] memoryTable;
        memoryTable = nullptr;
    }

    writeToFile();

    return 0;
}

// FUNCTION DEFINITIONS
// Helper function to calculate the 1D index.
int getIndex(short B, short R, short S, short maxStreak)
{
    int dimR = NUM_RED_CARDS + 1;
    int dimS = maxStreak + 1;

    return (B * dimR * dimS) + (R * dimS) + S;
}

void fillMemoryTable(double *memoryTable, short streakToWin)
{
    for (size_t i = 0; i <= NUM_BLACK_CARDS; i++)
    {
        for (size_t j = 0; j <= NUM_RED_CARDS; j++)
        {
            for (size_t k = 0; k <= streakToWin; k++)
            {
                memoryTable[getIndex(i, j, k, streakToWin)] = -1.0;
            }
        }
    }
}

double P(short B, short R, short S, double *memoryTable, short streakToWin)
{
    // Calculate 1D index
    int index = getIndex(B, R, S, streakToWin);

    // 1. Check memory FIRST!
    if (memoryTable[index] != -1.0) return memoryTable[index];

    double probability;

    // 2. Calculate the answer based on the state
    if (S == streakToWin)
    {
        probability = 1.0;
    }
    else if (B == 0 && R == 0)
    {
        probability = 0.0;
    }
    else
    {
        double rightBranch = 0.0;
        double wrongBranch = 0.0;

        if (B >= R)
        {
            // Guessing Black: Only calculate if those cards actually exist
            if (B > 0) rightBranch = ((double)B / (B + R)) * P(B - 1, R, S + 1, memoryTable, streakToWin);
            if (R > 0) wrongBranch = ((double)R / (B + R)) * P(B, R - 1, 0, memoryTable, streakToWin);
        }
        else
        {
            // Guessing Red: Only calculate if those cards actually exist
            if (R > 0) rightBranch = ((double)R / (B + R)) * P(B, R - 1, S + 1, memoryTable, streakToWin);
            if (B > 0) wrongBranch = ((double)B / (B + R)) * P(B - 1, R, 0, memoryTable, streakToWin);
        }

        probability = rightBranch + wrongBranch;
    }

    // 3. Save to memory and return
    memoryTable[index] = probability;

    return probability;
}

void writeToFile()
{
    // Create a CSV file for output.
    ofstream outFile(filesystem::path("markov_chain_results.csv"));

    // Catch errors with creating the file.
    if (!outFile.is_open())
    {
        cout << "Error: Could not create results.csv";
        return;
    }

    // Output header row.
    outFile << "streak_to_win,probability\n";

    for (size_t i = 0; i < MAX_STREAK; i++)
    {
        outFile << fixed << setprecision(15) << (i + 1) << ',' << results[i] << '\n';
    }

    outFile.close();
}