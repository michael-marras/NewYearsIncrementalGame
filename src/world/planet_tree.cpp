#include "world/planet_tree.h"

// Constructor
PlanetTree::PlanetTree(Planet* p) {
    root = new PlanetNode(0, p);
    nextPlanetId = 1;
}

// Destructor
PlanetTree::~PlanetTree() {
    DeleteNodeRecursive(root);
    root = nullptr;
}

void PlanetTree::DeleteNodeRecursive(PlanetNode* node) {
    if (!node) return; 

    DeleteNodeRecursive(node->left);
    
    DeleteNodeRecursive(node->right);
    
    // Delete the planet object first, then the node
    if (node->planet) {
        delete node->planet;
    }
    delete node;
}

// Add a child planet to a parent - takes ownership of the planet pointer
int PlanetTree::AddChild(int parentId, Planet* childPlanet) {
    PlanetNode* parent = FindPlanet(parentId);
    if (!parent) return -1;

    if (parent->left && parent->right) return -1;

    int childId = nextPlanetId;
    if (!parent->left) {
        parent->left = new PlanetNode(nextPlanetId, childPlanet);
    } else {
        parent->right = new PlanetNode(nextPlanetId, childPlanet);
    }
    nextPlanetId += 1;
    return childId;
}

// Find planet by ID - simple iterative approach using parent calculation
PlanetNode* PlanetTree::FindPlanet(int planetId) {
    if (!root) return nullptr;
    if (planetId == 0) return root;
    
    PlanetNode* current = root;
    int currentId = 0;
    
    while (currentId != planetId) {
        int leftChildId = 2 * currentId + 1;
        int rightChildId = 2 * currentId + 2;

        if (planetId == leftChildId) {
            return current->left ? current->left : nullptr;
        }
        if (planetId == rightChildId) {
            return current->right ? current->right : nullptr;
        }

        int ancestor = planetId;
        while (ancestor > rightChildId) {
            int parent = ancestor / 2;
            if (ancestor % 2 == 0) {
                parent--;
            }
            ancestor = parent;
        }

        if (ancestor == leftChildId) {
            if (!current->left) return nullptr;
            current = current->left;
            currentId = leftChildId;
        } else {
            if (!current->right) return nullptr;
            current = current->right;
            currentId = rightChildId;
        }
    }
    
    return current;
}
