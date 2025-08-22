#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "utils.h"

using namespace std;
using namespace lbcrypto;

// Decision tree node structure
struct TreeNode {
    double threshold;           // Decision threshold
    double input_value;         // Input value to compare against
    double comparison_result;   // Result of comparison (0 or 1)
    bool is_leaf;              // Whether this is a leaf node
    int level;                 // Level in the tree (0-based)
    int node_id;               // Unique node identifier
    
    TreeNode(double thresh, double input, int lvl, int id, bool leaf = false) 
        : threshold(thresh), input_value(input), comparison_result(0.0), 
          is_leaf(leaf), level(lvl), node_id(id) {}
};

// Complete binary decision tree class
class CompleteBinaryDecisionTree {
private:
    vector<TreeNode> internal_nodes;  // Only internal nodes (for comparisons)
    vector<TreeNode> leaf_nodes;      // Only leaf nodes
    int depth;
    int num_internal_nodes;
    int num_leaf_nodes;
    
public:
    CompleteBinaryDecisionTree(int tree_depth) : depth(tree_depth) {
        num_internal_nodes = (1 << depth) - 1;  // 2^d - 1
        num_leaf_nodes = 1 << depth;             // 2^d
        
        // Initialize tree
        initializeTree();
    }
    
    void initializeTree() {
        // Create internal nodes (levels 0 to depth-1)
        int node_id = 0;
        for (int level = 0; level < depth; level++) {
            int nodes_in_level = 1 << level;  // 2^level nodes at this level
            
            for (int i = 0; i < nodes_in_level; i++) {
                // Generate random threshold and input value
                double threshold = (double)(rand() % 100) / 100.0;  // 0.0 to 1.0
                double input_value = (double)(rand() % 100) / 100.0;  // 0.0 to 1.0
                
                internal_nodes.emplace_back(threshold, input_value, level, node_id, false);
                node_id++;
            }
        }
        
        // Create leaf nodes (level depth)
        for (int i = 0; i < num_leaf_nodes; i++) {
            // Leaf nodes don't need thresholds, but we'll give them values for completeness
            double leaf_value = (double)(rand() % 100) / 100.0;
            leaf_nodes.emplace_back(0.0, leaf_value, depth, node_id, true);
            node_id++;
        }
    }
    
    // Simulate comparison operation (as if it were encrypted)
    double performComparison(const TreeNode& node) {
        // Simulate: if input > threshold, return 1, else return 0
        // In real FHE, this would be a comparison operation
        return (node.input_value > node.threshold) ? 1.0 : 0.0;
    }
    
    // Simulate multiplication operation (as if it were encrypted)
    double performMultiplication(double a, double b) {
        // Simulate encrypted multiplication
        return a * b;
    }
    
    // Get parent node index in internal_nodes array
    int getParentIndex(int child_index, int child_level) {
        if (child_level == 0) return -1; // Root has no parent
        
        // Calculate parent index based on complete binary tree structure
        int level_start = (1 << (child_level - 1)) - 1;
        int child_position_in_level = child_index - ((1 << child_level) - 1);
        int parent_position = child_position_in_level / 2;
        
        return level_start + parent_position;
    }
    
    // Get path from root to a specific leaf
    vector<int> getPathToLeaf(int leaf_index) {
        vector<int> path;
        
        // Start from leaf and work backwards to root
        int current_level = depth;
        int current_position = leaf_index;
        
        // Build path from leaf to root, then reverse
        while (current_level > 0) {
            // Calculate node index in internal_nodes array
            int level_start = (1 << (current_level - 1)) - 1;
            int parent_position = current_position / 2;
            int parent_index = level_start + parent_position;
            
            path.push_back(parent_index);
            
            current_position = parent_position;
            current_level--;
        }
        
        // Reverse to get root-to-leaf path
        reverse(path.begin(), path.end());
        return path;
    }
    
    // Evaluate decision tree and count operations
    pair<int, int> evaluateTree() {
        int comparison_count = 0;
        int multiplication_count = 0;
        
        cout << "\n=== Decision Tree Evaluation ===" << endl;
        cout << "Tree Depth: " << depth << endl;
        cout << "Internal Nodes: " << num_internal_nodes << " (2^" << depth << " - 1)" << endl;
        cout << "Leaf Nodes: " << num_leaf_nodes << " (2^" << depth << ")" << endl << endl;
        
        // Step 1: Perform comparisons at all internal nodes
        cout << "Step 1: Performing comparisons at all internal nodes..." << endl;
        for (int i = 0; i < num_internal_nodes; i++) {
            internal_nodes[i].comparison_result = performComparison(internal_nodes[i]);
            comparison_count++;
            
            cout << "Internal Node " << i << " (Level " << internal_nodes[i].level << "): ";
            cout << "Input=" << fixed << setprecision(3) << internal_nodes[i].input_value;
            cout << ", Threshold=" << internal_nodes[i].threshold;
            cout << ", Comparison=" << internal_nodes[i].comparison_result << endl;
        }
        
        // Step 2: Calculate path products for each leaf
        cout << "\nStep 2: Calculating path products for each leaf..." << endl;
        
        for (int leaf_idx = 0; leaf_idx < num_leaf_nodes; leaf_idx++) {
            vector<int> path = getPathToLeaf(leaf_idx);
            double path_product = 1.0;
            
            cout << "Leaf " << leaf_idx << " path: ";
            
            // Print and calculate path product
            for (size_t i = 0; i < path.size(); i++) {
                int node_idx = path[i];
                double comp_result = internal_nodes[node_idx].comparison_result;
                
                cout << "Node" << node_idx << "(" << comp_result << ")";
                if (i < path.size() - 1) cout << " × ";
                
                if (i == 0) {
                    path_product = comp_result;
                } else {
                    path_product = performMultiplication(path_product, comp_result);
                    multiplication_count++;
                }
            }
            
            cout << " = " << fixed << setprecision(6) << path_product << endl;
        }
        
        return {comparison_count, multiplication_count};
    }
    
    // Print tree structure
    void printTreeStructure() {
        cout << "\n=== Tree Structure ===" << endl;
        
        // Print internal nodes
        for (int level = 0; level < depth; level++) {
            int nodes_in_level = 1 << level;
            int start_idx = (1 << level) - 1;
            
            cout << "Level " << level << " (Internal, " << nodes_in_level << " nodes): ";
            for (int i = 0; i < nodes_in_level; i++) {
                int node_idx = start_idx + i;
                cout << node_idx;
                if (i < nodes_in_level - 1) cout << " ";
            }
            cout << endl;
        }
        
        // Print leaf nodes
        cout << "Level " << depth << " (Leaves, " << num_leaf_nodes << " nodes): ";
        for (int i = 0; i < num_leaf_nodes; i++) {
            cout << "L" << i;
            if (i < num_leaf_nodes - 1) cout << " ";
        }
        cout << endl;
    }
    
    // Get theoretical operation counts
    pair<int, int> getTheoreticalCounts() {
        int theoretical_comparisons = num_internal_nodes;  // 2^d - 1 comparisons
        int theoretical_multiplications = num_leaf_nodes * (depth - 1);  // 2^d * (d-1) multiplications
        
        return {theoretical_comparisons, theoretical_multiplications};
    }
};



void DecisionTree(uint32_t depth, uint32_t integerBits) {
    lbcrypto::OpenFHEParallelControls.Disable();
    
    cout << "Integer Bits: " << integerBits << endl;            
    SetupCryptoContext(24, 128, integerBits);

    // Prepare test data - generate random arrays of length g_numValues
    vector<double> x1(g_numValues);
    vector<double> x2(g_numValues);
    vector<double> x3(g_numValues);
    
    // Initialize random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1 << (integerBits / 2));
    
    // Generate random values
    for (size_t i = 0; i < g_numValues; ++i) {
        x1[i] = dis(gen);
        x2[i] = dis(gen);
        x3[i] = dis(gen);
    }

    // Encode and encrypt
    Plaintext ptxt1 = g_cc->MakeCKKSPackedPlaintext(x1);
    Plaintext ptxt2 = g_cc->MakeCKKSPackedPlaintext(x2);
    Plaintext ptxt3 = g_cc->MakeCKKSPackedPlaintext(x3);

    auto c1 = g_cc->Encrypt(g_keys.publicKey, ptxt1);
    auto c2 = g_cc->Encrypt(g_keys.publicKey, ptxt2);
    auto c3 = g_cc->Encrypt(g_keys.publicKey, ptxt3);
   
    auto t_start = chrono::steady_clock::now();
    // Multiplication on CKKS
    auto cMult = g_cc->EvalMult(c1, c2);
    cMult = g_cc->Rescale(cMult);

    // Comparison CKKS - FHEW
    auto cResult = Comparison(cMult, c3);

    // Convert FHEW sign results back to CKKS
    auto t_fhew2ckks_start = chrono::steady_clock::now();
    auto cSignResult = g_cc->EvalFHEWtoCKKS(cResult, g_numValues, g_numValues);
    auto t_fhew2ckks_end = chrono::steady_clock::now();
    double t_fhew2ckks_sec = chrono::duration<double>(t_fhew2ckks_end - t_fhew2ckks_start).count();
    cout << "FHEW -> CKKS switching time: " << t_fhew2ckks_sec << " s" << endl;

    // Multiplication on CKKS
    auto t_mult2_start = chrono::steady_clock::now();
    auto cMult2 = g_cc->EvalMult(cSignResult, c3);
    cMult2 = g_cc->Rescale(cMult2);
    auto t_mult2_end = chrono::steady_clock::now();
    double t_mult2_sec = chrono::duration<double>(t_mult2_end - t_mult2_start).count();
    cout << "CKKS multiplication time: " << t_mult2_sec << " s" << endl;

    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();
    cout << "Total time: " << t_sec << " s" << endl;

    srand(42);  // Fixed seed for reproducible results
        
    cout << "\n" << string(80, '=') << endl;
    cout << "TESTING DECISION TREE WITH DEPTH " << depth << endl;
    cout << string(80, '=') << endl;
    
    CompleteBinaryDecisionTree tree(depth);
    tree.printTreeStructure();
    
    auto [actual_comparisons, actual_multiplications] = tree.evaluateTree();
    auto [theoretical_comparisons, theoretical_multiplications] = tree.getTheoreticalCounts();
    
    cout << "\n=== Operation Counts ===" << endl;
    cout << "Comparisons: " << actual_comparisons << " (theoretical: " << theoretical_comparisons << ")" << endl;
    cout << "Multiplications: " << actual_multiplications << " (theoretical: " << theoretical_multiplications << ")" << endl;

    cout << "Total time: " << t_sec * (actual_comparisons + actual_multiplications) << " s" << endl;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << "========== Decision Tree Depth 2 ==========" << endl;
    DecisionTree(6, 6);
    DecisionTree(6, 8);
    DecisionTree(6, 12);
    DecisionTree(6, 16);

    cout << "========== Decision Tree Depth 4 ==========" << endl;
    DecisionTree(8, 6);
    DecisionTree(8, 8);
    DecisionTree(8, 12);
    DecisionTree(8, 16);
    
    cout << "========== Decision Tree Depth 6 ==========" << endl;
    DecisionTree(12, 6);
    DecisionTree(12, 8);
    DecisionTree(12, 12);
    DecisionTree(12, 16);
    
    cout << "========== Decision Tree Depth 8 ==========" << endl;
    DecisionTree(16, 6);
    DecisionTree(16, 8);
    DecisionTree(16, 12);
    DecisionTree(16, 16);
    
    return 0;
}