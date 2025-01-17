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
    // list owners
    printf("\nExisting Pokedexes:\n");
    const OwnerNode *current = ownerHead;
    int index = 1;
    while (current->next != current) {
        printf("%d .%s\n", index, current->ownerName);
        index++;
        current = current->next;
    }

    if (current->next == current) {
        printf("%d .%s\n", index, current->ownerName);
    }

    printf("Choose a Pokedex by number: \n");
    scanf("%d", &index);
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
    int id;
    printf("Enter ID to add: \n");
    scanf("%d", &id);
    //found an id
    if (searchPokemonBFS(owner->pokedexRoot, id) != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return;
    }

    PokemonNode *root = owner->pokedexRoot;
    PokemonNode *newNode = createPokemonNode(&pokedex[id - 1]);
    insertPokemonNode(root, newNode);
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
    int id;
    printf("Enter Pokemon ID to release:\n");
    scanf("%d", &id);

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
            free(root);
            return temp;
        }
        if (root->right == NULL) {
            PokemonNode *temp = root->left;
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
    int firstId;
    int secondId;
    printf("Enter ID of the first Pokemon: ");
    scanf("%d", &firstId);
    printf("Enter ID of the second Pokemon: ");
    scanf("%d", &secondId);
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

//working but add case of already evolved and trying to evolve again
void evolvePokemon(OwnerNode *owner) {
    PokemonNode *root = owner->pokedexRoot;
    if (root == NULL) {
        printf("Pokedex is empty\n");
        return;
    }
    int id;
    printf("Enter ID of Pokemon to evolve: ");
    scanf("%d", &id);

    PokemonNode *node = searchPokemonBFS(root, id);
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

    PokemonNode *newNode = createPokemonNode(&pokedex[oldId + 1]);
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newNode);
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", oldName, oldId, newNode->data->name,
           newNode->data->id);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu() {
    //head of empty list of owners
    OwnerNode *ownerHead = NULL;
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
            // case 3:
            //     deletePokedex();
            //     break;
            // case 4:
            //     mergePokedexMenu();
            //     break;
            // case 5:
            //     sortOwners();
            //     break;
            // case 6:
            //     printOwnersCircular();
            //     break;
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
    OwnerNode *current = ownerHead;
    while (current != NULL) {
        freePokemonTree(current->pokedexRoot);
        freeOwnerNode(current);
    }
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
    free(owner->pokedexRoot);
    free(owner->ownerName);
    free(owner->next);
    free(owner->prev);
}

void openPokedexMenu(void) {
    printf("Your name:\n");
    char *ownerName = getDynamicInput();
    if (findOwnerByName(ownerName) != NULL) {
        printf("Owner %s already exists. Not creating a new Pokedex.\n", ownerName);
        return;
    }
    int starterIndex;
    printf("Choose starter:\n");
    for (int i = 1; i < STARTERS + 1; i++) {
        printf("%d. %s\n", i, pokedex[i - 1].name);
    }

    scanf("%d", &starterIndex);
    PokemonNode *pokemonNode = createPokemonNode(&pokedex[starterIndex - 1]);
    OwnerNode *newOwner = createOwner(ownerName, pokemonNode);
    addOwner(newOwner);
}

OwnerNode *findOwnerByName(const char *name) {
    OwnerNode *current = ownerHead;
    if (current == NULL) {
        return NULL;
    }
    //if only one owner
    if (current->next == current) {
        if (strcmp(current->ownerName, name) == 0) {
            return current;
        }
        return NULL;
    }
    //get to last owner which points to itself
    while (current->next != current) {
        if (strcmp(current->ownerName, name) == 0) {
            return current;
        }
        current = current->next;
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

    //malloc for all owner's details. Owner name was already malloced
    owner->pokedexRoot = malloc(sizeof(PokemonNode));
    if (owner->ownerName == NULL || owner->pokedexRoot == NULL) {
        free(owner);
        exit(1);
    }

    owner->ownerName = ownerName;
    owner->next = owner;
    owner->prev = findLastOwner();

    strcpy(owner->ownerName, ownerName);
    owner->pokedexRoot = starter;

    return owner;
}

OwnerNode *findLastOwner() {
    OwnerNode *current = ownerHead;
    if (current == NULL) {
        return NULL;
    }
    while (current->next != current) {
        current = current->next;
    }

    return current;
}

void addOwner(OwnerNode *newOwner) {
    OwnerNode *current = ownerHead;
    //if new owner is first
    if (current == NULL) {
        ownerHead = newOwner;
        //only one so points to himself
        ownerHead->next = ownerHead;
        ownerHead->prev = ownerHead;
        return;
    }
    //get to last owner
    while (current->next != current) {
        current = current->next;
    }

    current->next = newOwner;
}
