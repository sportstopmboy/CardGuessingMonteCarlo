#include <iostream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <fstream>

// VARIABLE DECLARATIONS
// Declare and initialise constants which control game parameters.
const short NUM_CARDS = 52;
const short STREAK_TO_WIN = 26;
const unsigned long NUM_GAMES = 100000000L;
const short NUM_GAMES_DIGITS = floor(log10(NUM_GAMES));

// Array which stores results in order for file output.
float results[STREAK_TO_WIN]{};

// Fetch number of CPU cores and limit to half to limit CPU usage to 50%.
const unsigned short NUM_CORES = std::thread::hardware_concurrency() / 2;
// Atomic integer to store number of streaks calculated.
std::atomic<short> currentStreakTask{1};

// Use to protect std::cout
std::mutex printMutex;

// FUNCTION DECLARATIONS
void fillDeck(short deck[]);
void shuffleDeck(short deck[], std::mt19937 &gen);
short makeGuess(short numBlackCards, short numRedCards);
float playGame(short streakToWin);
void worker();

// MAIN FUNCTION
int main()
{
    std::cout << "Starting simulation using " << NUM_CORES << " CPU cores...\n\n";

    // Declare vector containing the threads which will excecute.
    std::vector<std::thread> workers;

    // Create threads.
    for (size_t i = 0; i < NUM_CORES; i++)
    {
        workers.push_back(std::thread(worker));
    }

    // Once done, close each thread.
    for (std::thread &worker : workers)
    {
        worker.join();
    }

    std::cout << "All simulations complete! Writing to file...\n";

    // Create a CSV file for output.
    std::ofstream outFile(std::filesystem::path("results.csv"));

    // Catch errors with creating the file.
    if (!outFile.is_open())
    {
        std::cout << "Error: Could not create results.csv";
    }

    // Output each result on a new line.
    for (size_t i = 0; i < STREAK_TO_WIN; i++)
    {
        outFile << std::fixed << std::setprecision(NUM_GAMES_DIGITS) << (i + 1) << "," << results[i] << '\n';
    }

    // Close the file after writing.
    outFile.close();

    return 0;
}

// FUNCTION DEFINITIONS
void fillDeck(short deck[])
{
    // Fill the deck with an equal amount of red and black cards.
    // A 0 element represents a red card.
    // A 1 element represents a black card.
    for (size_t i = 0; i < NUM_CARDS / 2; i++)
    {
        deck[i] = 0;
    }
    for (size_t i = NUM_CARDS / 2; i < NUM_CARDS; i++)
    {
        deck[i] = 1;
    }
}

void shuffleDeck(short deck[], std::mt19937 &gen)
{
    // Shuffle the deck.
    std::shuffle(deck, deck + NUM_CARDS, gen);
}

short makeGuess(short numBlackCards, short numRedCards)
{
    // Calculates which card has a higher probability of appearing next and guesses that one.
    return numBlackCards >= numRedCards ? 1 : 0;
}

float playGame(short streakToWin)
{
    // Variables controlling randomness.
    std::random_device rd;
    std::mt19937 gen(rd());

    // Each thread gets its own private deck of cards.
    short deck[NUM_CARDS]{};
    // Variable to count how many times the game is won.
    unsigned long wins = 0;

    // Play the game the specified amount of times.
    for (size_t j = 0; j < NUM_GAMES; j++)
    {

        // Set counters for how many cards of each colour are in the deck.
        short numBlackCards = NUM_CARDS / 2;
        short numRedCards = NUM_CARDS / 2;

        // Prepare the deck.
        fillDeck(deck);
        shuffleDeck(deck, gen);

        // Declare win condition variables.
        short guesses = 0;
        short streak = 0;

        while (guesses < NUM_CARDS && streak < streakToWin)
        {
            // Make guess based off of probability.
            short guess = makeGuess(numBlackCards, numRedCards);

            // Update variables given the guess is a black card and correct.
            if (guess == deck[guesses] && deck[guesses] == 1)
            {
                numBlackCards--;
                guesses++;
                streak++;
            }
            // Update variables given the guess is a red card and correct.
            else if (guess == deck[guesses] && deck[guesses] == 0)
            {
                numRedCards--;
                guesses++;
                streak++;
            }
            // Update variables given the guess is incorrect.
            else
            {
                guesses++;
                streak = 0;
            }
        }

        // Tally the win if the game was won.
        if (streak == streakToWin)
        {
            wins++;
        }
    }

    // Output collected data after all games have been played.
    std::lock_guard<std::mutex> lock(printMutex);
    std::cout << "STREAK TO WIN: " << streakToWin << '\n';
    std::cout << "Number of Wins: " << wins << '\n';
    std::cout << "Win %: " << std::fixed << std::setprecision(NUM_GAMES_DIGITS) << ((float)wins / NUM_GAMES) << "\n\n";

    return ((float)wins / NUM_GAMES);
}

void worker()
{
    while (true)
    {
        // Grab the next available streak number and increment the dispenser.
        short streakToCalculate = currentStreakTask.fetch_add(1);

        // If we've processed all streaks, this thread's job is done.
        if (streakToCalculate > STREAK_TO_WIN)
        {
            break;
        }

        // Run the game for the ticket we just pulled and store result in array.
        results[streakToCalculate - 1] = playGame(streakToCalculate);
    }

}
