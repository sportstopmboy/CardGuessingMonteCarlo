# SolvingCardGuessingGame
<h2>Game Rules</h2>
<p>You are given a standard deck of 52 playing cards which have been shuffled. At the beginning of the game, you make a prediction about how many card colors you are going to be able to correctly predict in a row. Next, you guess the color of the next card in the deck. After you make your guess you draw a card to see if your guess was correct. If your guess was correct, you add one point to your streak of correct guesses. If your guess was incorrect, you reset your streak. You repeat this process until you either run out of cards or your streak reaches the original predicition that you set. If you run out of cards, it means you have lost; and if you correctly predicted your streak, you win.</p>
<h2>Problem Statement</h2>
<p>The question then is: <b>What is the most card colors you can predict whilst expecting to win the majority of games?</b></p>
<h2>Solution</h2>
<h3>The Strategy</h3>
<p>The first step to solving this problem is finding the optimal strategy for playing this game. Although it seems like it's all up to chance, there is a way to get an edge (the effect of this edge is illustrated later). That edge is in guessing the color which has show up less until this point. Since the number of cards in limited, even if only one more red card has shown up thus far, it is more likely that the next card will be black. This means that the chance is not 50/50 each time, since each probability is affected by the previous result. The optimal strategy, therefore, is:
&nbsp<li>As the first guess, pick a random color.</li>
&nbsp<li>If the card is black, your next guess is red; and vice versa.</li>
&nbsp<li>Repeat this process, where each guess is the color which has been seen less until this point. And if the count is equal, pick a card at random.</li></p>
<h3>The Model</h3>
<p>To actually solve this problem, I modelled the problem as a Markov Chain. The model of the problem is as follows:</p>
<h4>The State</h4>
<h3>The Result</h3>
<p>The result shows that the answer to the question in the problem statement is 6. On average, you can expect to win a bit more than half of games played when you make the prediction 6 cards in a row and you play it perfectly. The results for the win percentages for the other streak prediction values are stored in the example 'markov_chain_results.csv' file.</p>
<h2>The Monte Carlo Simulation</h2>
<p>I also wrote a Monte Carlo simulation to prove the result obtained was indeed correct. The Monte Carlo simulation simulated a player who follows the optimal strategy. This player counts its wins and and calculates its win percentage over millions of games. The program is multithreaded to optimise runtime, and it exports the values into a CSV for later data comparison and analysis.</p>
<h2>Further Data Interpretation</h2>
<p>When we plot the results of the Monte Carlo simulation, we can see that the results clearly follow a logistic decay function.</p>
