#include <iostream>
#include <vector>
#include "WifiChannel.h"
#include "Node.h"

const Time JITTER_TIME = 1e2;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <numNodes> <simTime>" << std::endl;
        return -1;
    }

    int numNodes = atoi(argv[1]);
    Time simTime = atoi(argv[2]) * 1e6;
    int run = atoi(argv[3]);

    Simulator::Configure(simTime, run);

    WifiChannel channel;
    Node ap{&channel, 6};
    std::vector<Node> nodes;
    nodes.reserve(numNodes * 2);
    for (int i = 0; i < numNodes; i++) {
        nodes.emplace_back(&channel, 6);
        Simulator::Schedule(Simulator::GenerateUniformRandomVariable(0, numNodes * JITTER_TIME),
                            std::bind(&Node::Send, &nodes[i], &ap));
    }

    Simulator::Run();

    std::cout.precision(std::numeric_limits<double>::max_digits10);
    std::cout << run << "\t" << numNodes << "\t" << 8.0 * ap.GetRxBytes() / simTime << std::endl;
    for (const auto& node : nodes) {
        //std::cout << 8.0 * node.GetTxBytes() / simTime << std::endl;
    }

    return 0;
}