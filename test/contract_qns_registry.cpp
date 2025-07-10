#define NO_UEFI

#include "contract_testing.h"

class ContractTestingQNSRegistry : public ContractTesting {
public:
    ContractTestingQNSRegistry() {
        initEmptySpectrum();
        initEmptyUniverse();
        INIT_CONTRACT(QNS_REGISTRY);
        callSystemProcedure(QNS_REGISTRY_CONTRACT_INDEX, INITIALIZE);
    }

    QNS_REGISTRY* getState()
    {
        return (QNS_REGISTRY*)contractStates[QNS_REGISTRY_CONTRACT_INDEX];
    }

    void beginEpoch(bool expectSuccess = true)
    {
        callSystemProcedure(QNS_REGISTRY_CONTRACT_INDEX, BEGIN_EPOCH, expectSuccess);
    }

    void endEpoch(bool expectSuccess = true)
    {
        callSystemProcedure(QNS_REGISTRY_CONTRACT_INDEX, END_EPOCH, expectSuccess);
    }
};

// Test contract initialization
TEST(QNSRegistryTest, TestInitialization) {
    ContractTestingQNSRegistry test;

    // Check that root node exists
    QNS_REGISTRY::NodeExists_input input;
    QNS_REGISTRY::NodeExists_output output;
    memset(&output, 0, sizeof(output));

    input.node = QNS_REGISTRY::QUBIC_ROOT_NODE;

    test.callFunction(QNS_REGISTRY_CONTRACT_INDEX, 5, input, output); // NodeExists is function 5

    EXPECT_TRUE(output.exists);

    // Check total names count
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1u);

    // Check root node owner
    QNS_REGISTRY::GetOwner_input ownerInput;
    QNS_REGISTRY::GetOwner_output ownerOutput;
    memset(&ownerOutput, 0, sizeof(ownerOutput));

    ownerInput.node = QNS_REGISTRY::QUBIC_ROOT_NODE;
    test.callFunction(QNS_REGISTRY_CONTRACT_INDEX, 1, ownerInput, ownerOutput); // GetOwner is function 1

    EXPECT_TRUE(ownerOutput.exists);
    EXPECT_EQ(ownerOutput.owner, m256i(QNS_REGISTRY_CONTRACT_INDEX, 0, 0, 0)); // SELF
}
