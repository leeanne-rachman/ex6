#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
# define STARTERS 3
#define MAX_POKEMONS 151

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str) {
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0) {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int) strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src) {
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *) malloc(len + 1);
    if (!dest) {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt) {
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success) {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0) {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int) strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0') {
            printf("Invalid input.\n");
        } else {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type) {
    switch (type) {
        case GRASS:
            return "GRASS";
        case FIRE:
            return "FIRE";
        case WATER:
            return "WATER";
        case BUG:
            return "BUG";
        case NORMAL:
            return "NORMAL";
        case POISON:
            return "POISON";
        case ELECTRIC:
            return "ELECTRIC";
        case GROUND:
            return "GROUND";
        case FAIRY:
            return "FAIRY";
        case FIGHTING:
            return "FIGHTING";
        case PSYCHIC:
            return "PSYCHIC";
        case ROCK:
            return "ROCK";
        case GHOST:
            return "GHOST";
        case DRAGON:
            return "DRAGON";
        case ICE:
            return "ICE";
        default:
            return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput() {
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *) malloc(capacity);
    if (!input) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (size + 1 >= capacity) {
            capacity *= 2;
            char *temp = (char *) realloc(input, capacity);
            if (!temp) {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char) c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node) {
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("Pokedex is empty.\n");

        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice) {
        case 1:
            displayBFS(owner->pokedexRoot);
            break;
        case 2:
            preOrderTraversal(owner->pokedexRoot);
            break;
        case 3:
            inOrderTraversal(owner->pokedexRoot);
            break;
        case 4:
            postOrderTraversal(owner->pokedexRoot);
            break;
        case 5:
            displayAlphabetical(owner->pokedexRoot);
            break;
        default:
            printf("Invalid choice.\n");
    }
}

void displayBFS(PokemonNode *root) {
    BFSGeneric(root, printPokemonNode);
}

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }

    // Create a queue for BFS
    Queue *queue = createQueue();
    enQueue(queue, root);
    while (queue->front != NULL) {
        PokemonNode *current = deQueue(queue);
        // Call function pointer
        visit(current);

        if (current->left != NULL)
            enQueue(queue, current->left);

        if (current->right != NULL)
            enQueue(queue, current->right);
    }

    free(queue);
}

Queue *createQueue() {
    Queue *myq = malloc(sizeof(Queue));
    myq->front = NULL;
    myq->rear = NULL;

    return myq;
}

QueueNode *createNode(PokemonNode *node) {
    QueueNode *myNode = malloc(sizeof(QueueNode));
    if (myNode == NULL) {
        exit(1);
    }

    myNode->node = node;
    myNode->next = NULL;

    return myNode;
}

void enQueue(Queue *queue, PokemonNode *node) {
    QueueNode *queueNode = createNode(node);
    if (queue->rear == NULL) {
        queue->front = queueNode;
        queue->rear = queueNode;

        return;
    }

    queue->rear->next = queueNode;
    queue->rear = queueNode;
}

PokemonNode *deQueue(Queue *queue) {
    if (queue->front == NULL) {
        printf("Queue empty");
        exit(1);
    }

    QueueNode *myn = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    PokemonNode *node = myn->node;
    free(myn);

    return node;
}

void preOrderTraversal(PokemonNode *root) {
    preOrderGeneric(root, printPokemonNode);
}

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }

    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}

void inOrderTraversal(PokemonNode *root) {
    inOrderGeneric(root, printPokemonNode);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }

    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}

void postOrderTraversal(PokemonNode *root) {
    postOrderGeneric(root, printPokemonNode);
}

void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }

    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}

void displayAlphabetical(PokemonNode *root) {
    NodeArray nodeArray;

    initNodeArray(&nodeArray, MAX_POKEMONS);
    collectAll(root, &nodeArray);
    //sort
    qsort(nodeArray.nodes, nodeArray.size, sizeof(PokemonNode *), compareByNameNode);
    //print
    for (int i = 0; i < nodeArray.size; i++) {
        printPokemonNode(nodeArray.nodes[i]);
    }
    //free
    free(nodeArray.nodes);
}

void initNodeArray(NodeArray *na, int cap) {
    na->nodes = (PokemonNode **) malloc(cap * sizeof(PokemonNode *));
    if (na->nodes == NULL) {
        exit(1);
    }

    na->capacity = cap;
    na->size = 0;
}

void collectAll(PokemonNode *root, NodeArray *na) {
    if (root == NULL) {
        return;
    }

    addNode(na, root);
    collectAll(root->left, na);
    collectAll(root->right, na);
}

void addNode(NodeArray *na, PokemonNode *node) {
    if (na->size >= na->capacity) {
        //doubles the capacity of the NodeArray to ensure there's enough space for future nodes
        na->capacity *= 2;
        na->nodes = (PokemonNode **) realloc(na->nodes, na->capacity * sizeof(PokemonNode *));
        if (na->nodes == NULL) {
            exit(1);
        }
    }
    na->nodes[na->size++] = node;
}

int compareByNameNode(const void *a, const void *b) {
    PokemonNode *nodeA = *(PokemonNode **) a;
    PokemonNode *nodeB = *(PokemonNode **) b;

    return strcmp(nodeA->data->name, nodeB->data->name);
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// // --------------------------------------------------------------
void enterExistingPokedexMenu() {
    if(ownerHead == NULL) {
        printf("No existing Pokedexes.\n");
        return;
    }

    printf("\nExisting Pokedexes:\n");
    printAllOwners();
    int index = readIntSafe("Choose a Pokedex by number: ");
    OwnerNode *pokedex = ownerHead;
    for (int i = 0; i < index - 1; i++) {
        pokedex = pokedex->next;
    }

    printf("\nEntering %s's Pokedex...\n", pokedex->ownerName);
    int subChoice;
    do {
        printf("\n-- %s's Pokedex Menu --\n", pokedex->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice) {
            case 1:
                addPokemon(pokedex);
                break;
            case 2:
                displayMenu(pokedex);
                break;
            case 3:
                freePokemon(pokedex);
                break;
            case 4:
                pokemonFight(pokedex);
                break;
            case 5:
                evolvePokemon(pokedex);
                break;
            case 6:
                printf("Back to Main Menu.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

void addPokemon(OwnerNode *owner) {
    int id = readIntSafe("Enter ID to add: ");
    //found an id
    if (searchPokemonBFS(owner->pokedexRoot, id) != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return;
    }

    //index minus 1 because array index starts from 0
    PokemonNode *newNode = createPokemonNode(&pokedex[id - 1]);
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newNode);
    printf("Pokemon %s (ID %d) added.\n", pokedex[id - 1].name, id);
}

PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    //empty tree or found the id
    if (root == NULL || root->data->id == id) {
        return root;
    }

    //If id is less than current node, search left son
    if (id < root->data->id) {
        return searchPokemonBFS(root->left, id);
    }

    //If id is greater than current node, search right son
    return searchPokemonBFS(root->right, id);
}

PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    //if tree is empty
    if (root == NULL) {
        root = newNode;
        return root;
    }
    //insert to left son
    if (root->data->id > newNode->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    }
    //insert to right son
    else if (root->data->id < newNode->data->id) {
        root->right = insertPokemonNode(root->right, newNode);
    }

    return root;
}

void freePokemon(OwnerNode *owner) {
    //check if empty
    if (!owner->pokedexRoot) {
        printf("No Pokemon to release.\n");
        return;
    }

    int id = readIntSafe("Enter Pokemon ID to release: ");
    PokemonNode *nodeToRemove = searchPokemonBFS(owner->pokedexRoot, id);

    if (nodeToRemove) {
        printf("Removing Pokemon %s (ID %d).\n", nodeToRemove->data->name, id);
        owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, id);
    } else {
        printf("Pokemon with ID %d not found.\n", id);
    }
}

PokemonNode *removePokemonByID(PokemonNode *root, int id) {
    if (root == NULL) {
        return root;
    }

    if (id < root->data->id) {
        root->left = removePokemonByID(root->left, id);
    } else if (id > root->data->id) {
        root->right = removePokemonByID(root->right, id);
    } else {
        // Case 1: No children or one child
        if (root->left == NULL) {
            PokemonNode *temp = root->right;
            free(root->data);
            free(root);
            return temp;
        }
        if (root->right == NULL) {
            PokemonNode *temp = root->left;
            free(root->data);
            free(root);
            return temp;
        }

        // Case 2: Two children
        // Find the smallest in the right subtree
        PokemonNode *temp = findMinInSubTree(root->right);
        root->data = temp->data;
        root->right = removePokemonByID(root->right, temp->data->id);
    }

    return root;
}

PokemonNode *findMinInSubTree(PokemonNode *root) {
    PokemonNode *current = root;
    while (current && current->left != NULL) {
        current = current->left;
    }

    return current;
}

void pokemonFight(OwnerNode *owner) {
    if (owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }

    int firstId = readIntSafe("Enter ID of the first Pokemon: ");
    int secondId = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode *first = searchPokemonBFS(owner->pokedexRoot, firstId);
    PokemonNode *second = searchPokemonBFS(owner->pokedexRoot, secondId);

    if (first == NULL || second == NULL) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }

    const double firstAttack = (first->data->attack) * 1.5;
    const double firstHp = (first->data->hp) * 1.2;
    const double firstSum = firstAttack + firstHp;

    const double secondAttack = (second->data->attack) * 1.5;
    const double secondHp = (second->data->hp) * 1.2;
    const double secondSum = secondAttack + secondHp;

    printf("Pokemon 1: %s (Score = %.2f)\nPokemon 2: %s (Score = %.2f)\n", first->data->name, firstSum,
           second->data->name, secondSum);
    if (firstSum > secondSum) {
        printf("%s wins!\n", first->data->name);
    } else if (secondSum > firstSum) {
        printf("%s wins!\n", second->data->name);
    } else {
        printf("Its a tie!\n");
    }
}

void evolvePokemon(OwnerNode *owner) {
    PokemonNode *root = owner->pokedexRoot;
    if (root == NULL) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }

    int id = readIntSafe("Enter ID of Pokemon to evolve: ");
    PokemonNode *node = searchPokemonBFS(root, id);
    PokemonNode *evolvedForm = searchPokemonBFS(root, id + 1);
    //if has evolved form
    if (evolvedForm != NULL) {
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n",
               id + 1, evolvedForm->data->name, node->data->name, id);
        printf("Removing Pokemon %s (ID %d).\n", node->data->name, id);
        owner->pokedexRoot = removePokemonByID(root, id);

        return;
    }
    if (node == NULL) {
        printf("No pokemon with ID  %d found\n", id);

        return;
    }
    if (node->data->CAN_EVOLVE == CANNOT_EVOLVE) {
        printf("%s (ID %d) cannot evolve.\n", node->data->name, node->data->id);

        return;
    }

    char *oldName = node->data->name;
    int oldId = id;
    printf("Removing Pokemon %s (ID %d).\n", oldName, oldId);
    owner->pokedexRoot = removePokemonByID(root, oldId);

    PokemonNode *newNode = createPokemonNode(&pokedex[oldId]);
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newNode);
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", oldName, oldId, newNode->data->name,
           newNode->data->id);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu() {
    //head of empty list of owners
    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice) {
            case 1:
                openPokedexMenu();
                break;
            case 2:
                enterExistingPokedexMenu();
                break;
            case 3:
                deletePokedex();
                break;
            case 4:
                mergePokedexMenu();
                break;
            case 5:
                sortOwners();
                break;
            case 6:
                printOwnersCircular();
                break;
            case 7:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main() {
    mainMenu();
    freeAllOwners();
    return 0;
}

void freeAllOwners(void) {
    //nothing to release
    if (ownerHead == NULL) {
        return;
    }

    OwnerNode *current = ownerHead;
    OwnerNode *next;

    do {
        next = current->next;
        freePokemonTree(current->pokedexRoot);
        freeOwnerNode(current);
        current = next;
    } while (current != ownerHead);

    ownerHead = NULL;
}

void freePokemonTree(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    //free children
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    //free data and itself
    free(root->data);
    free(root);
}

void freeOwnerNode(OwnerNode *owner) {
    if (owner == NULL) {
        return;
    }

    free(owner->ownerName);
    free(owner);
    owner = NULL;
}

void openPokedexMenu(void) {
    printf("Your name: ");
    char *ownerName = getDynamicInput();
    if (findOwnerByName(ownerName) != NULL) {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", ownerName);
        free(ownerName);
        return;
    }

    int index = 0;
    int growthIndex = 0;
    printf("Choose Starter:\n");
    for (int i = 1; i < STARTERS + 1; i++) {
        printf("%d. %s\n", i, pokedex[growthIndex].name);
        growthIndex = growthIndex + STARTERS;
        index++;
    }

    index = readIntSafe("");
    PokemonNode *pokemonNode;
    if (index == 1) {
        pokemonNode = createPokemonNode(&pokedex[index - 1]);
    } else if (index == 2) {
        pokemonNode = createPokemonNode(&pokedex[index + 1]);
    } else {
        pokemonNode = createPokemonNode(&pokedex[index + STARTERS]);
    }

    OwnerNode *newOwner = createOwner(ownerName, pokemonNode);
    addOwner(newOwner);
    printf("Your choice: New Pokedex created for %s with starter %s.", newOwner->ownerName, pokemonNode->data->name);

    free(ownerName);
}

OwnerNode *findOwnerByName(const char *name) {
    OwnerNode *current = ownerHead;
    if (current == NULL) {
        return NULL;
    }
    //if only one owner
    if (current->next == ownerHead) {
        if (strcmp(current->ownerName, name) == 0) {
            return current;
        }

        return NULL;
    }
    //get to last owner which points to itself
    while (current->next != ownerHead) {
        if (strcmp(current->ownerName, name) == 0) {
            return current;
        }

        current = current->next;
    }
    if (strcmp(current->ownerName, name) == 0) {
        return current;
    }

    return NULL;
}

PokemonNode *createPokemonNode(const PokemonData *data) {
    PokemonNode *newNode = malloc(sizeof(PokemonNode));
    if (newNode == NULL) {
        exit(1);
    }

    PokemonData *pokemonData = malloc(sizeof(PokemonData));
    if (pokemonData == NULL) {
        free(newNode);
        exit(1);
    }

    *pokemonData = *data;
    newNode->data = pokemonData;

    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    OwnerNode *owner = malloc(sizeof(OwnerNode));
    if (owner == NULL) {
        exit(1);
    }
    owner->ownerName = malloc(strlen(ownerName) + 1);
    if (owner->ownerName == NULL) {
        exit(1);
    }

    owner->pokedexRoot = starter;
    strcpy(owner->ownerName, ownerName);

    owner->next = NULL;
    owner->prev = NULL;

    return owner;
}

OwnerNode *findLastOwner() {
    OwnerNode *current = ownerHead;
    if (current == NULL) {
        return NULL;
    }
    while (current->next != ownerHead) {
        current = current->next;
    }

    return current;
}

void addOwner(OwnerNode *newOwner) {
    //if new owner is first
    if (ownerHead == NULL) {
        ownerHead = newOwner;
        //only one so points to himself
        ownerHead->next = ownerHead;
        ownerHead->prev = ownerHead;
        return;
    }

    OwnerNode *lastOwner = findLastOwner();
    // Connect new owner to list
    newOwner->next = ownerHead;
    newOwner->prev = lastOwner;
    // Connect list to new owner
    lastOwner->next = newOwner;
    ownerHead->prev = newOwner;
}

void printAllOwners() {
    if (ownerHead == NULL) {
        printf("No existing Pokedexes.\n");

        return;
    }

    const OwnerNode *current = ownerHead;
    int index = 1;

    do {
        printf("%d. %s\n", index, current->ownerName);
        current = current->next;
        index++;
    } while (current != ownerHead);
}

void deletePokedex(void) {
    //no head
    if (ownerHead == NULL) {
        printf("No existing Pokedexes to delete.\n");

        return;
    }

    printf("\n=== Delete a Pokedex ===\n");
    printAllOwners();
    int index = readIntSafe("Choose a Pokedex to delete by number: ");

    OwnerNode *current = ownerHead;
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }

    printf("Deleting %s's entire Pokedex...\n", current->ownerName);
    removePokedex(current);
    printf("Pokedex deleted.\n");
}

void removePokedex(OwnerNode *owner) {
    // Free the Pokemon tree first
    if (owner->pokedexRoot != NULL) {
        freePokemonTree(owner->pokedexRoot);
        owner->pokedexRoot = NULL;
    }

    // If only one owner
    if (owner->next == owner && owner->prev == owner) {
        ownerHead = NULL;
        freeOwnerNode(owner);
        return;
    }

    // Save necessary pointers before modifying anything
    OwnerNode *nextNode = owner->next;
    OwnerNode *prevNode = owner->prev;

    // If deleting head of list
    if (owner == ownerHead) {
        ownerHead = nextNode;
    }

    prevNode->next = nextNode;
    nextNode->prev = prevNode;

    freeOwnerNode(owner);
}

void mergePokedexMenu(void) {
    //check if no owners or one owner
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }

    printf("\n=== Merge Pokedexes ===\nEnter name of first owner: ");
    char *firstName = getDynamicInput();;
    printf("Enter name of second owner: ");
    char *secondName = getDynamicInput();;

    OwnerNode *firstOwner = findOwnerByName(firstName);
    OwnerNode *secondOwner = findOwnerByName(secondName);

    if (firstOwner == NULL || secondOwner == NULL) {
        printf("One or both owners not found.\n");
        return;
    }

    printf("Merging %s and %s...\n", firstName, secondName);
    mergePokedexes(firstOwner, secondOwner);
    removePokedex(secondOwner);
    printf("Merge completed.\nOwner '%s' has been removed after merging.\n", secondName);
    free(firstName);
    free(secondName);
}

void mergePokedexes(OwnerNode *firstOwner, OwnerNode *secondOwner) {
    Queue *queue = createQueue();
    PokemonNode *originalRoot = secondOwner->pokedexRoot;
    enQueue(queue, secondOwner->pokedexRoot);

    while (queue->front != NULL) {
        PokemonNode *current = deQueue(queue);
        if (current->left) {
            enQueue(queue, current->left);
        }
        if (current->right) {
            enQueue(queue, current->right);
        }

        // Create new node and insert into first owner's tree
        PokemonNode *newNode = createPokemonNode(current->data);
        firstOwner->pokedexRoot = insertPokemonNode(firstOwner->pokedexRoot, newNode);
    }

    // Free  second owner's tree after merging
    freePokemonTree(originalRoot);
    secondOwner->pokedexRoot = NULL;
    free(queue);
}

void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    char *tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;
    PokemonNode *tempRoot = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = tempRoot;
}

void sortOwners(void) {
    // If the list is empty or contains only one element
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }

    int swapped;
    do {
        swapped = 0;
        OwnerNode *current = ownerHead;

        do {
            if (strcmp(current->ownerName, current->next->ownerName) > 0) {
                char *tempName = current->ownerName;
                current->ownerName = current->next->ownerName;
                current->next->ownerName = tempName;

                PokemonNode *tempPokedexRoot = current->pokedexRoot;
                current->pokedexRoot = current->next->pokedexRoot;
                current->next->pokedexRoot = tempPokedexRoot;
                swapped = 1;
            }
            current = current->next;
        } while (current->next != ownerHead);
    } while (swapped);
    printf("Owners sorted by name.\n");
}

void printOwnersCircular(void) {
    if (ownerHead == NULL) {
        printf("No owners.\n");
        return;
    }

    char direction;
    printf("Enter direction (F or B): ");
    scanf(" %c", &direction);
    while (direction != 'f' && direction != 'F' && direction != 'b' && direction != 'B') {
        printf("Invalid direction, must be F or B.\n");
        scanf("%c", &direction);
    }

    OwnerNode *current = ownerHead;
    scanf("%*c");
    int prints = readIntSafe("How many prints? ");
    if (direction == 'f' || direction == 'F') {
        for (int i = 0; i < prints; i++) {
            printf("[%d] %s\n", i + 1, current->ownerName);
            current = current->next;
        }

        return;
    }

    //print b
    for (int i = 0; i < prints; i++) {
        printf("[%d] %s\n", i + 1, current->ownerName);
        current = current->prev;
    }
}
