#include <bits/stdc++.h>
using namespace std;

struct Item {
    int value;
    int weight;
};

struct Individual {
    vector<bool> chromosome;
    int fitness;

    Individual(int size) : chromosome(size), fitness(0) {}
};

using Population = vector<Individual>;

// Hardcoded problem
const int CAPACITY = 15;
const vector<Item> ITEMS = {
    {12, 4},
    {10, 6},
    {8, 5},
    {11, 7},
    {14, 3}
};
const int ITEM_COUNT = ITEMS.size();

// EA parameters
const int POP_SIZE = 30;
const int GENERATIONS = 100;
const double MUTATION_RATE = 0.1;
const int TOURNAMENT_SIZE = 3;

// Helper: evaluate fitness
int evaluate(const Individual& ind) {
    int totalValue = 0, totalWeight = 0;
    for (int i = 0; i < ITEM_COUNT; ++i) {
        if (ind.chromosome[i]) {
            totalValue += ITEMS[i].value;
            totalWeight += ITEMS[i].weight;
        }
    }
    return (totalWeight <= CAPACITY) ? totalValue : 0;
}

// Helper: create random individual
Individual randomIndividual(mt19937& rng) {
    Individual ind(ITEM_COUNT);
    for (int i = 0; i < ITEM_COUNT; ++i)
        ind.chromosome[i] = uniform_int_distribution<>(0, 1)(rng);
    ind.fitness = evaluate(ind);
    return ind;
}

// Selection: tournament
Individual tournamentSelect(const Population& pop, mt19937& rng) {
    int bestIdx = -1;
    int bestFit = -1;
    for (int i = 0; i < TOURNAMENT_SIZE; ++i) {
        int idx = uniform_int_distribution<>(0, POP_SIZE - 1)(rng);
        if (pop[idx].fitness > bestFit) {
            bestFit = pop[idx].fitness;
            bestIdx = idx;
        }
    }
    return pop[bestIdx];
}

// Crossover: single-point
pair<Individual, Individual> crossover(const Individual& p1, const Individual& p2, mt19937& rng) {
    int point = uniform_int_distribution<>(1, ITEM_COUNT - 1)(rng);
    Individual c1 = p1, c2 = p2;
    for (int i = point; i < ITEM_COUNT; ++i) {
        swap(c1.chromosome[i], c2.chromosome[i]);
    }
    return {c1, c2};
}

// Mutation
void mutate(Individual& ind, mt19937& rng) {
    for (int i = 0; i < ITEM_COUNT; ++i) {
        if (uniform_real_distribution<>(0, 1)(rng) < MUTATION_RATE) {
            ind.chromosome[i] = !ind.chromosome[i];
        }
    }
}

// Evolve one island
void evolveIsland(Population& pop, mt19937& rng) {
    Population newPop;
    while (newPop.size() < pop.size()) {
        Individual parent1 = tournamentSelect(pop, rng);
        Individual parent2 = tournamentSelect(pop, rng);
        auto [child1, child2] = crossover(parent1, parent2, rng);
        mutate(child1, rng);
        mutate(child2, rng);
        child1.fitness = evaluate(child1);
        child2.fitness = evaluate(child2);
        newPop.push_back(child1);
        if (newPop.size() < pop.size())
            newPop.push_back(child2);
    }
    pop = move(newPop);
}

// Print best solution
void printBest(const Population& pop) {
    auto best = *max_element(pop.begin(), pop.end(), [](const Individual& a, const Individual& b) {
        return a.fitness < b.fitness;
    });
    cout << "Best fitness: " << best.fitness << "\n";
    cout << "Items: ";
    for (int i = 0; i < ITEM_COUNT; ++i)
        if (best.chromosome[i])
            cout << i << " ";
    cout << endl;
}

int main() {
    mt19937 rng(random_device{}());

    int numIslands = 1;
    vector<Population> islands(numIslands, Population(POP_SIZE));

    // Initialize islands
    for (int i = 0; i < numIslands; ++i)
        for (int j = 0; j < POP_SIZE; ++j)
            islands[i][j] = randomIndividual(rng);

    // Evolve each island
    for (int gen = 0; gen < GENERATIONS; ++gen) {
        for (int i = 0; i < numIslands; ++i) {
            evolveIsland(islands[i], rng);
        }
    }

    // Print best from island 0
    printBest(islands[0]);

    return 0;
}
