#include <iostream>
#include <cmath>
#include <map>
#include <list>
#include <cstdlib>
#include <vector>
#include <ctime>
#define INF 1000000
using namespace std;

int parent[1000000];
struct fib_node{
    int degree; //no. children
    fib_node* parent;
    fib_node* child;
    fib_node* left;
    fib_node* right;
    int key; //distance to source
    bool mark; 
    int index; //pointer to node
};

struct fib_heap{
    fib_node* min_node;
    int n_nodes;
    fib_heap(){
        min_node = NULL;
        n_nodes = 0;
    }
};

fib_node* create_node(int key){
    fib_node* x = new fib_node;
    x->key = key;
    return x;
}

fib_heap* create_heap(){
    fib_heap* new_fib_heap = new fib_heap;
    new_fib_heap->n_nodes = 0;
    new_fib_heap->min_node = NULL;
    return new_fib_heap;
}

void insert(fib_heap* H, fib_node* x, int key){
    fib_node* min_node = H->min_node;
    x->key = key;
    x->degree = 0;
    x->parent = NULL;
    x->child = NULL;
    x->left = x;
    x->right = x;
    x->mark = false;
    
    if(min_node!=NULL){
        (min_node->left)->right = x; //add to left min node
        x->right = min_node;
        x->left = min_node->left;
        min_node->left = x;
    }

    if(min_node==NULL || x->key < min_node->key){
        H->min_node = x; //update min node
    }

    H->n_nodes = H->n_nodes + 1; //increase num nodes
}

void remove_node_from_root(fib_node *node){
    if(node->left!=node){ // check if node has left/right sibling
        node->left->right = node->right;
        node->right->left = node->left;
    }

    if(node->parent!=NULL){
        int parent_degree = node->parent->degree;
        if(parent_degree==1){
            node->parent->child = NULL;
        } else{
            node->parent->child = node->left; //update to child node 
        }

        node->parent->degree = node->parent->degree - 1; //update parent node
    }
}

void fib_link(fib_heap* H, fib_node* high_node, fib_node* low_node){
    remove_node_from_root(high_node);
    if(low_node->degree==0){ // check if low node has no child
        low_node->child = high_node;
        high_node->left = high_node;
        high_node->right = high_node;
        high_node->parent = low_node;
    } else{
        fib_node* child_node = low_node->child; // add to the left child node of low node
        child_node->left->right = high_node;
        high_node->right = child_node;
        child_node->left = high_node;
        high_node->left = child_node->left;
    }
    high_node->parent = low_node; // update parent of high node
    low_node->degree = low_node->degree + 1;

    high_node->mark = false; // update mark
}

fib_heap* fib_union(fib_heap* fib1, fib_heap* fib2){
    fib_heap* H = create_heap(); // create new heap
    fib_node* min_node_1 = fib1->min_node;
    fib_node* min_node_2 = fib2->min_node;

    if(fib1->min_node==NULL){ // check if heap is NULL
        H->min_node = min_node_2;
    } else if(fib2->min_node==NULL){
        H->min_node = min_node_1;
    } else{ // concat root list
        min_node_1->left = min_node_2->left;
        (min_node_2->left)->right = min_node_1;
        min_node_2->left = min_node_1->left;
        (min_node_1->left)->right = min_node_2;

        if(min_node_1->key < min_node_2->key){ // check to update min node
            H->min_node = min_node_1;
        } else{
            H->min_node = min_node_2;
        }
    }

    H->n_nodes = fib1->n_nodes + fib2->n_nodes;
    return H;
}

void add_node_to_root(fib_heap *H, fib_node *new_node){
    fib_node *min_node = H->min_node; // create new node
    new_node->parent = NULL;
    new_node->mark = false;

    if(min_node!=NULL){ // check if heap not NULL
        fib_node *min_left = min_node->left;
        min_node->left = new_node; // add to the left of min node
        new_node->right = min_node;
        new_node->left = min_left;
        min_left->right = new_node;

        if(min_node->key > new_node->key){
            H->min_node = new_node;
        }
    } else { // if heap is NULL
        H->min_node = new_node;
        new_node->left = new_node;
        new_node->right = new_node;
    }
}

void add_child(fib_node *parent_node, fib_node *child_node){
    if(parent_node->degree == 0){
        parent_node->child = child_node;
        child_node->parent = parent_node;
        child_node->left = child_node;
        child_node->right = child_node;
    } else{
        fib_node* real_child = parent_node->child;
        fib_node* real_child_left = real_child->left;
        real_child->left = child_node;
        child_node->right = real_child;
        child_node->left = real_child_left;
        real_child_left->right = child_node;
    }

    child_node->parent = parent_node;
    parent_node->degree++;
}

void fib_consolidate(fib_heap* H){
    int node_degree;
    int count=0, root_count=0;

    if(H->n_nodes>1){
        int degree_table_size = H->n_nodes; //create an array store degree node in root list
        vector<fib_node*> degree_table;
        fib_node *curr_node=H->min_node, *start_node=H->min_node;
        fib_node *exist_degree_node, *curr_conso_node; // node used when do condolidata
        fib_node *tmp_node=H->min_node, *ite_node=H->min_node; // tmp node when create array

        do{
            root_count++;
            ite_node = ite_node->right; // ite over all node in root list
        } while(ite_node!=tmp_node);

        while(count<root_count){
            curr_conso_node = curr_node; // node will be viewed
            curr_node = curr_node->right; //ite
            node_degree = curr_conso_node->degree;
            
            while(true){
                while(node_degree >= degree_table.size()){
                    degree_table.push_back(NULL);
                }

                if(degree_table[node_degree]==NULL){ // add node to array if not node has this degree
                    degree_table[node_degree]=curr_conso_node;
                    break;
                } else{
                    exist_degree_node = degree_table[node_degree]; // node actual in array
                    if(curr_conso_node->key > exist_degree_node->key){ // swap 2 node
                        tmp_node = curr_conso_node;
                        curr_conso_node = exist_degree_node;
                        exist_degree_node = tmp_node;
                    }
                    
                    if(exist_degree_node==curr_conso_node){
                        break;
                    }

                    fib_link(H, exist_degree_node, curr_conso_node); // call fib_link

                    degree_table[node_degree] = NULL; // degree has changed
                    node_degree++; // update degree
                }
            }
            count++;
        }


        H->min_node = NULL;

        for(int i=0; i<degree_table.size(); i++){ // create root list
            if(degree_table[i]!=NULL){
                // degree_table[i]->left = degree_table[i];
                // degree_table[i]->right = degree_table[i];

                // if(H->min_node!=NULL){
                //     (H->min_node)->left->right = degree_table[i];
                //     degree_table[i]->right = H->min_node;
                //     degree_table[i]->left = (H->min_node)->left;
                //     (H->min_node)->left = degree_table[i];

                //     if(degree_table[i]->key < (H->min_node)->key){
                //         H->min_node = degree_table[i];
                //     }
                // } else{
                //     H->min_node = degree_table[i];
                // }

                // if(H->min_node == NULL){
                //     H->min_node = degree_table[i];
                // } else if(degree_table[i]->key < (H->min_node)->key){
                //     H->min_node = degree_table[i];
                // }
                add_node_to_root(H, degree_table[i]);
            }
        }
    }
}

fib_node* fib_heap_extract_min(fib_heap* H){
    fib_node* min_node = H->min_node; //get min node

    if(min_node!=NULL){
        int degree = min_node->degree;
        fib_node* curr_node = min_node->child;
        fib_node* rm_node;
        int count=0;

        while(count<degree){ // add child of min node to root
            rm_node = curr_node;
            curr_node = curr_node->right;
            add_node_to_root(H, rm_node);
            count++;
        }

        (min_node->left)->right = min_node->right; // remove min node from heap
        (min_node->right)->left = min_node->left;
        H->min_node = min_node->right; // new min node in right 
        H->n_nodes = H->n_nodes - 1; // update num node

        if(H->n_nodes == 0){
            H->min_node=NULL;
        } else{ // call consolidate
            H->min_node = min_node->right;
            fib_consolidate(H);
        }
    }
    return min_node;
}

void cut(fib_heap* H, fib_node* node, fib_node* parent_node){
    remove_node_from_root(node); // remove node
    add_node_to_root(H, node); // add to root
}

void cascading_cut(fib_heap* H, fib_node* node){
    fib_node* parent_node = node->parent;
    if(parent_node!=NULL){
        if(node->mark == false){ // check if node has be marked
            node->mark = true;
        } else{
            cut(H, node, parent_node);
            cascading_cut(H, parent_node); //ite 
        }
    }
}

void fib_heap_decrease_key(fib_heap* H, fib_node* node, int k){
    if(k > node->key){ // check decrease condition
        return;
    }

    node->key = k; // update new key
    fib_node* parent_node = node->parent;

    if(parent_node!=NULL && node->key < parent_node->key){
        cut(H, node, parent_node);
        cascading_cut(H, parent_node);
    }

    if(node->key < (H->min_node)->key){
        H->min_node = node; // update min node
    }
}

void fib_heap_delete(fib_heap* H, fib_node* node){
    fib_heap_decrease_key(H, node, -1);
    fib_heap_extract_min(H);
}

struct adj_node{
    int dst_node;
    int weight;
};

struct graph{
    int num_vertices;
    map<int, list<adj_node*>> adjacent_list_map; // wiehgt
};


graph* create_graph(int graph_size){
    graph* G = new graph;
    G->num_vertices = graph_size;
    return G;
}

void add_edge(graph* G, int src, int dst, int weight){ // add in 2 vertice list
    adj_node* new_node = new adj_node;
    new_node->dst_node = dst;
    new_node->weight = weight;
    G->adjacent_list_map[src].push_back(new_node);

    adj_node* new_node_2 = new adj_node;
    new_node_2->dst_node = src;
    new_node_2->weight = weight;
    G->adjacent_list_map[dst].push_back(new_node_2);
}

// void print_path(int parent[], int dst){
//     if(parent[dst]==-1){
//         cout<<-1<<"---";
//         return;
//     }
    
//     print_path(parent, parent[dst]);
//     cout<<dst<<" ";
// }


bool dijkstra(graph* G, int src, int dst){
    fib_node** node_array = new fib_node*[G->num_vertices]; //distance to src
    bool *marked = new bool[G->num_vertices]; //reach min distance
    fib_heap* H = new fib_heap;
    // int parent[G->num_vertices]; //save previous node in shortest path
    
    // int count_marked = 0;

    // build heap, one vertice one node
    for(int i=1; i<=G->num_vertices; i++){
        parent[i] = -1;
        marked[i] = false;
        node_array[i] = new fib_node;

        if(i==src){
            insert(H, node_array[i], 0);
        } else{
            insert(H, node_array[i], INF);
        }

        node_array[i]->index = i;
    }

   
    fib_node* min_node; // node has min distance
    int min_node_index; // index of node
    // while(count_marked<G->num_vertices){
    for(int j=0; j<G->num_vertices; j++){ 
        min_node = fib_heap_extract_min(H); //extract min distance
        
        if(min_node!=NULL){
            min_node_index = min_node->index; // get node
            if(min_node->key == INF && min_node_index == dst) return false; //check if has shortest path
            marked[min_node_index] = true;
            // printf("%d\n", min_node_index);

            list<adj_node*> adj_list_for_node = G->adjacent_list_map[min_node_index];
            for(list<adj_node*>::iterator it=adj_list_for_node.begin(); it!=adj_list_for_node.end(); ++it){
                int adj_node_index = (*it)->dst_node;
                if(marked[adj_node_index]==false){
                    if(node_array[adj_node_index]->key>node_array[min_node_index]->key+(*it)->weight){
                        parent[adj_node_index] = min_node_index; // save parent
                        fib_heap_decrease_key(H, node_array[adj_node_index], 
                        node_array[min_node_index]->key+(*it)->weight); // update distance
                    }
                }
            }
        }
    }
    // print_path(parent, dst);
    // printf("\n");
    return true;
}




int n, m;
int main(){
    clock_t Time, Start=clock();
    int u, v, w;

    scanf("%d %d", &n, &m);
    graph* G = create_graph(n);
   
    while(m--){
        scanf("%d %d %d", &u, &v, &w);
        add_edge(G, u, v, w);
    }

    if(dijkstra(G, 1, n)){
        vector<int> path;
        for(int v=n; v!=-1; v=parent[v]) path.push_back(v);
        for(int i = path.size()-1; i > 0; --i) printf("%d ", path[i]);
        printf("%d\n", path[0]);
    } else puts("-1");
    // print_path(parent, n-1);
}


