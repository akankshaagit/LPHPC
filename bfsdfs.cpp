1.Design and implement Parallel Breadth First Search and Depth First Search based on existing
algorithms using OpenMP. Use a Tree or an undirected graph for BFS and DFS.


#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

// Define a structure for an undirected graph
class Graph {
    int V; // Number of vertices
    vector<vector<int>> adj; // Adjacency list

public:
    // Constructor to initialize the graph with a given number of vertices
    Graph(int V) : V(V) {
        adj.resize(V);
    }

    // Function to add an edge between two vertices
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Since the graph is undirected
    }

    // Breadth First Search
    void parallelBFS(int start) {
        vector<bool> visited(V, false); // Keep track of visited vertices
        queue<int> q; // Queue for BFS traversal
        q.push(start); // Enqueue the starting vertex
        visited[start] = true; // Mark the starting vertex as visited

        // Perform BFS traversal
        while (!q.empty()) {
            int size = q.size(); // Get the current size of the queue
            #pragma omp parallel for
            for (int i = 0; i < size; ++i) {
                int u;
                #pragma omp critical
                {
                    u = q.front(); // Get the front element of the queue
                    q.pop(); // Dequeue the front element
                }
                cout << u << " "; // Print the current vertex
                // Traverse adjacent vertices of the current vertex
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        #pragma omp critical
                        {
                            visited[v] = true; // Mark the adjacent vertex as visited
                            q.push(v); // Enqueue the adjacent vertex
                        }
                    }
                }
            }
        }
    }

    // Depth First Search
    void parallelDFS(int start) {
        vector<bool> visited(V, false); // Keep track of visited vertices
        stack<int> s; // Stack for DFS traversal
        s.push(start); // Push the starting vertex onto the stack

        // Perform DFS traversal
        #pragma omp parallel
        {
            while (!s.empty()) {
                int u = -1; // Initialize u to an invalid value
                #pragma omp critical
                {
                    if (!s.empty()) {
                        u = s.top(); // Get the top element of the stack
                        s.pop(); // Pop the top element
                    }
                }
                if (u != -1 && !visited[u]) {
                    visited[u] = true; // Mark the current vertex as visited
                    cout << u << " "; // Print the current vertex
                    // Traverse adjacent vertices of the current vertex
                    for (int v : adj[u]) {
                        if (!visited[v]) {
                            #pragma omp critical
                            {
                                s.push(v); // Push the adjacent vertex onto the stack
                            }
                        }
                    }
                }
            }
        }
    }
};

int main() {
    // Create a graph
    int V, E;
    cout << "Enter the number of vertices and edges: ";
    cin >> V >> E;

    Graph graph(V); // Instantiate a graph with V vertices

    // Input edges of the graph
    cout << "Enter the edges (format: source destination):" << endl;
    for (int i = 0; i < E; ++i) {
        int u, v;
        cin >> u >> v;
        graph.addEdge(u, v); // Add the edge between vertices u and v
    }

    // Perform BFS and DFS
    int startVertex;
    cout << "Enter the starting vertex for BFS and DFS: ";
    cin >> startVertex;

    cout << "BFS Traversal: ";
    graph.parallelBFS(startVertex); // Perform parallel BFS traversal
    cout << endl;

    cout << "DFS Traversal: ";
    graph.parallelDFS(startVertex); // Perform parallel DFS traversal
    cout << endl;

    return 0;
}


output -
Enter the number of vertices and edges: 6 7
Enter the edges (format: source destination):
0 1
0 2
1 3
1 4
2 4
3 5
4 5
Enter the starting vertex for BFS and DFS: 0
BFS Traversal: 0 1 2 3 4 5 
DFS Traversal: 0 2 4 5 3 1 


#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

// Define a Node structure for the tree
struct Node {
    int data;
    vector<Node*> children;

    Node(int val) : data(val) {}
};

// Breadth First Search
void parallelBFS(Node* root) {
    queue<Node*> q;
    q.push(root);

    while (!q.empty()) {
        int size = q.size(); // Get the current size of the queue

        // Create a temporary vector to store children
        vector<Node*> temp;

        #pragma omp parallel for shared(q, temp)
        for (int i = 0; i < size; ++i) {
            Node* current;
            #pragma omp critical
            {
                current = q.front(); // Get the front element of the queue
                q.pop(); // Dequeue the front element
            }
            cout << current->data << " "; // Print the current node data

            // Store children in the temporary vector
            for (Node* child : current->children) {
                #pragma omp critical
                temp.push_back(child);
            }
        }

        // Enqueue children after traversing the current level
        for (Node* child : temp) {
            q.push(child); // Enqueue the child node
        }
    }
}

// Depth First Search
void parallelDFS(Node* root) {
    stack<Node*> s;
    s.push(root);

    while (!s.empty()) {
        // Create a temporary vector to store children 
        vector<Node*> temp;

        // Process the current node
        Node* current;
        #pragma omp critical
        {
            current = s.top(); // Get the top element of the stack
            s.pop(); // Pop the top element
        }
        cout << current->data << " "; // Print the current node data

        // Store children in the temporary vector
        for (Node* child : current->children) {
            temp.push_back(child); // Store the child node in the temporary vector
        }

        // Push children onto the stack in reverse order
        for (int j = temp.size() - 1; j >= 0; --j) {
            Node* child = temp[j];
            #pragma omp critical
            s.push(child); // Push the child node onto the stack
        }
    }
}

int main() {
    // Creating a sample tree
    Node* root = new Node(1);
    root->children.push_back(new Node(2));
    root->children.push_back(new Node(3));
    root->children[0]->children.push_back(new Node(4));
    root->children[0]->children.push_back(new Node(5));
    root->children[1]->children.push_back(new Node(6));
    root->children[1]->children.push_back(new Node(7));
    root->children.push_back(new Node(20));
    root->children[1]->children.push_back(new Node(8));

    // Perform BFS and DFS on the tree
    cout << "BFS: ";
    parallelBFS(root); // Perform parallel BFS traversal
    cout << endl;

    cout << "DFS: ";
    parallelDFS(root); // Perform parallel DFS traversal
    cout << endl;

    return 0;
}


output:
BFS: 1 2 3 20 4 5 6 7 8 
DFS: 1 2 4 5 3 6 7 8 20 


____________________________________________________________________________________________________________________________________________________



question and answers:


Q: What is the main idea of BFS and DFS?
A: BFS explores neighbors level by level; DFS explores as far as possible along each branch before backtracking.

Q: What are the differences in memory usage between BFS and DFS?
A: BFS uses more memory due to the queue storing all nodes at a level; DFS uses less, storing the path via a stack.

Q: Why is parallelizing BFS challenging?
A: It requires level-wise traversal. Ensuring threads access correct nodes without conflicts is hard with queues.

Q: Why do you use critical sections in OpenMP here?
A: To avoid race conditions when multiple threads modify shared data structures like queues, stacks, or visited[].

Q: What are the drawbacks of using #pragma omp critical?
A: It can become a bottleneck as it serializes access, reducing parallel performance.

Q: How is the tree different from a graph in terms of traversal?
A: A tree has no cycles and only one path between nodes, so no visited[] is needed. Graphs may have cycles and require tracking.

Q: How does your BFS implementation ensure nodes aren’t visited twice?
A: Using a visited[] array, updated inside a critical section.

Q: Why do you push children in reverse order in DFS?
A: To maintain left-to-right traversal order, mimicking recursive DFS.

Q: What data structures are used for BFS and DFS?
A: BFS: queue (FIFO); DFS: stack (LIFO).

Q: What happens if visited[] is not marked critical?
A: Two threads could try to visit the same node simultaneously, leading to duplicate processing.

Q: Can this be implemented without OpenMP?
A: Yes, but it would be strictly sequential.

Q: What alternatives exist to #pragma omp critical for better performance?
A: Use omp atomic, thread-local storage, or parallel queues (with locks or concurrent data structures).

Q: How would you improve parallelism in DFS?
A: Assign different DFS calls to threads for disconnected components or divide tree branches among threads.

Q: How would you handle large graphs?
A: Use dynamic scheduling, distribute graphs across threads, or use graph libraries like Boost Graph or GraphX.

Q: What happens if the input graph is not connected?
A: Only one connected component is traversed. You'd need to loop through all nodes and start BFS/DFS on unvisited ones.

#pragma omp parallel: Run a block with multiple threads

#pragma omp for: Split loops across threads

#pragma omp critical: Prevent threads from accessing shared data at the same time (like locking)






