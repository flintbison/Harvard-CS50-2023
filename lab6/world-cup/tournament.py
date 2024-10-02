# Simulate a sports tournament.

# Import modules to support program.
# csv module to read csv files.
import csv
# sys module to access command line arguments.
import sys
# random module to generate random numbers to give an aspect of randomless to probability of simulated games.
import random

# Number of simulations to run.
N = 1000


def main():

    # Ensure correct usage, by looking for presence of at least 2 arguments at command line.
    if len(sys.argv) != 2:
        sys.exit("Usage: python tournament.py FILENAME")

    # Declare empty list to store teams from csv file.
    teams = []

    # Read teams into memory from csv file.
    # Get filename from commandline arguments.
    filename = sys.argv[1]

    # Keyword "with" automatically closes the file after the block.
    # Open filename in commandline argument [1] as variable name "file".
    with open(filename) as file:
        # Delcare variable reader and assign return value of dictionary reader function.
        reader = csv.DictReader(file)
        # Loop through each team in dictionary reader.
        for team in reader:
            # Access the rating of the current team, convert it to an integer explicitly by using int() function.
            team["rating"] = int(team["rating"])
            # Append converted team dictionary to teams list.
            teams.append(team)

    # Declare empty dictionary to store number of times each team has won in simulated tournaments.
    counts = {}

    # Simulate N tournaments and keep track of win counts
    # Perform a for loop which iterates for the number N.
    for i in range(N):
        # Declare variable called sim_winner to store the output of simulate_tournament, which should be the name of the overall winning team.
        sim_winner = simulate_tournament(teams)
        # Check if key for simulated winning team exists in counts dict.
        if sim_winner in counts:
            # If sim_winner already in counts dict, then increment key value by 1.
            counts[sim_winner] += 1
        else:
            # Else create sim_winner key with value 1.
            counts[sim_winner] = 1

    # Print each team's chances of winning, according to simulation
    for team in sorted(counts, key=lambda team: counts[team], reverse=True):
        print(f"{team}: {counts[team] * 100 / N:.1f}% chance of winning")


def simulate_game(team1, team2):
    """Simulate a game. Return True if team1 wins, False otherwise."""
    rating1 = team1["rating"]
    rating2 = team2["rating"]
    probability = 1 / (1 + 10 ** ((rating2 - rating1) / 600))
    return random.random() < probability


def simulate_round(teams):
    """Simulate a round. Return a list of winning teams."""
    winners = []

    # Simulate games for all pairs of teams
    for i in range(0, len(teams), 2):
        if simulate_game(teams[i], teams[i + 1]):
            winners.append(teams[i])
        else:
            winners.append(teams[i + 1])

    return winners


def simulate_tournament(teams):
    """Simulate a tournament. Return name of winning team."""

    # Perform a loop while the length of elements witin the list teams is more than 1.
    while len(teams) > 1:
        # For each loop, submit the list of teams to the simulate_round function, and reassign its return value (a list called winners) to variable teams.
        teams = simulate_round(teams)
    # Once the list teams is 1, break while loop condition and return the name of the overall winning team from the local variable list teams.
    return teams[0]["team"]


if __name__ == "__main__":
    main()
