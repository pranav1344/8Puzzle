#include<iostream>
#include<memory>
#include<string>
#include<deque>
#include<unordered_map>
#include<vector>
#include<algorithm>
#include<random>

enum Moves {
    UP,
    DOWN,
    LEFT,
    RIGHT
};
struct Node {
    std::string state;
    std::shared_ptr<Node> parent;

    Node(std::string state, std::shared_ptr<Node> parent = NULL ) {
        this->state = state;
        this->parent = parent;
    }
    bool isMoveValid(Moves move, int x, int y) {
        switch (move)
        {
        case UP:
            if(x != 0)
                return true;
            else return false;
            break;
        case DOWN:
            if(x != 2)
                return true;
            else return false;
            break;
        case LEFT:
            if(y != 0)
                return true;
            else return false;
            break;
        case RIGHT:
            if(y != 2)
                return true;
            else return false;
            break;
        default:
            std::cout<<"Illegal move";
            break;
        }
        return false;
    }
    std::vector<std::string> getNextState() {
        std::vector<std::string> states;
        int blank_tile = std::find(state.begin(), state.end(), '0') - state.begin();
        int x = blank_tile % 3;
        int y = blank_tile / 3;
        if(isMoveValid(UP, x, y)) {
            int tx = x-1;
            int ty = y;
            int tb = ty * 3 + tx;
            std::string next(state);
            next[blank_tile] = state[tb];
            next[tb] = '0';
            states.push_back(std::move(next));
        }
        if(isMoveValid(DOWN, x, y)) {
            int tx = x+1;
            int ty = y;
            int tb = ty * 3 + tx;
            std::string next(state);
            next[blank_tile] = state[tb];
            next[tb] = '0';
            states.push_back(std::move(next));
        }
        if(isMoveValid(LEFT, x, y)) {
            int tx = x;
            int ty = y-1;
            int tb = ty * 3 + tx;
            std::string next(state);
            next[blank_tile] = state[tb];
            next[tb] = '0';
            states.push_back(std::move(next));
        }
        if(isMoveValid(RIGHT, x, y)) {
            int tx = x;
            int ty = y+1;
            int tb = ty * 3 + tx;
            std::string next(state);
            next[blank_tile] = state[tb];
            next[tb] = '0';
            states.push_back(std::move(next));
        }
        return states;
    }
};

void printState(std::string state) {
    std::cout<<state[0]<<" "<<state[1]<<" "<<state[2]<<"\n";
    std::cout<<state[3]<<" "<<state[4]<<" "<<state[5]<<"\n";
    std::cout<<state[6]<<" "<<state[7]<<" "<<state[8]<<"\n- - -\n";
}

std::shared_ptr<Node> getRoot(std::shared_ptr<Node> n) {
   return n->parent ? getRoot(n->parent) : n;
}

std::shared_ptr<Node> initPath(std::shared_ptr<Node> source, std::shared_ptr<Node> dest) {
    while(dest) {
        std::shared_ptr<Node> temp = dest->parent;
        dest->parent = source;
        source = dest;
        dest = temp;
    }
    return source;
}

void constructPath(std::shared_ptr<Node> node, std::vector<std::string> * path) {
    while(node) {
        path->push_back(node->state);
        node = node->parent;
    }
    std::reverse(path->begin(), path->end());
}

void printPath(std::vector<std::string> & path) {
    for(auto i : path) {
        printState(i);
    }
}

bool biDirectional(std::string start, std::string dest, int * explored, int * finished, std::vector<std::string> * path) {
    std::shared_ptr<Node> start_node = std::make_shared<Node>(start);
    std::shared_ptr<Node> dest_node = std::make_shared<Node>(dest);
    std::unordered_map<std::string, std::shared_ptr<Node>> source_visited {{start, start_node}};
    std::unordered_map<std::string, std::shared_ptr<Node>> dest_visited {{dest, dest_node}};
    std::deque<std::shared_ptr<Node>> source_queue{start_node};
    std::deque<std::shared_ptr<Node>> dest_queue{dest_node};
    int nodes_exhausted = 0;
    //The code keeps looping until it runs out of states to explore
    while(!source_queue.empty()) {
        auto size_source = source_queue.size();
        while(size_source--) {
            auto curr = source_queue.front();
            source_queue.pop_front();
            //This stores the number of nodes that have been tried
            nodes_exhausted++;
            //Go over all the legal moves
            for (const auto & succ : curr->getNextState()) {
                //Store the parent node for this node
                auto m = std::make_shared<Node>(succ, curr);
                //Look into the set of visited nodes to see if this node has been visited
                auto i = dest_visited.find(succ);
                if(i != dest_visited.end()) {
                    // We have found the intersection node
                    auto p = i->second;
                    //Handler in case we reached destination node instead of intermediate node
                    if(getRoot(m)->state == dest) {
                        std::swap(m, p);
                    }
                    m = initPath(m, p->parent);
                    constructPath(m, path);
                    *explored = source_visited.size() + dest_visited.size();
                    *finished = nodes_exhausted;
                    return true;
                }
                if(source_visited.count(succ))
                    continue;
                source_visited[succ] = m;
                source_queue.push_back(m);
            }
        }
    }
    std::cout<<"\nTotal "<<nodes_exhausted<<" were tried \n";
    return false;
}

struct RNG {
    int operator () (int n) {
        return std::rand() / (1.0 + RAND_MAX) * n;
    }
};

int main() {
    std::string source;
    std::string destination;
    std::cin>>source>>destination;
    std::vector<std::string> path;
    std::cout<<"The source configuration is \n";
    printState(source);
    int explored = 0, finished = 0;
    bool solution_exists = biDirectional(source, destination, &explored, &finished, &path);
    if(solution_exists) {
        std::cout<<"A solution for given puzzle exists\n";
        printPath(path);
    }
    else {
        std::cout<<"Given puzzle is unsolvable\n";
    }

    std::cout<<"The destination configuration is \n";
    printState(destination);
    return 0;
}