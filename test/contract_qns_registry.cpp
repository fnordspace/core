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

    // Procedure helpers

    QNS_REGISTRY::SetSubnodeOwner_output createSubdomain(uint64 parentNode, uint64 labelHash, const id& owner, const id& caller) {
        QNS_REGISTRY::SetSubnodeOwner_input input;
        QNS_REGISTRY::SetSubnodeOwner_output output;
        memset(&output, 0, sizeof(output));
        input.parentNode = parentNode;
        input.labelHash = labelHash;
        input.owner = owner;
        invokeUserProcedure(QNS_REGISTRY_CONTRACT_INDEX, 3, input, output, caller, 0);
        return output;
    }

    QNS_REGISTRY::SetOwner_output transferOwnership(uint64 node, const id& newOwner, const id& caller) {
        QNS_REGISTRY::SetOwner_input input;
        QNS_REGISTRY::SetOwner_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        input.newOwner = newOwner;
        invokeUserProcedure(QNS_REGISTRY_CONTRACT_INDEX, 1, input, output, caller, 0);
        return output;
    }

    QNS_REGISTRY::SetResolver_output setResolver(uint64 node, const id& resolver, const id& caller) {
        QNS_REGISTRY::SetResolver_input input;
        QNS_REGISTRY::SetResolver_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        input.resolver = resolver;
        invokeUserProcedure(QNS_REGISTRY_CONTRACT_INDEX, 2, input, output, caller, 0);
        return output;
    }

    QNS_REGISTRY::SetTTL_output setTTL(uint64 node, uint32 ttl, const id& caller) {
        QNS_REGISTRY::SetTTL_input input;
        QNS_REGISTRY::SetTTL_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        input.ttl = ttl;
        invokeUserProcedure(QNS_REGISTRY_CONTRACT_INDEX, 4, input, output, caller, 0);
        return output;
    }

    // Function helpers  
    QNS_REGISTRY::NodeExists_output nodeExists(uint64 node) {
        QNS_REGISTRY::NodeExists_input input;
        QNS_REGISTRY::NodeExists_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        callFunction(QNS_REGISTRY_CONTRACT_INDEX, 5, input, output);
        return output;
    }

    QNS_REGISTRY::GetOwner_output getOwner(uint64 node) {
        QNS_REGISTRY::GetOwner_input input;
        QNS_REGISTRY::GetOwner_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        callFunction(QNS_REGISTRY_CONTRACT_INDEX, 1, input, output);
        return output;
    }

    QNS_REGISTRY::GetResolver_output getResolver(uint64 node) {
        QNS_REGISTRY::GetResolver_input input;
        QNS_REGISTRY::GetResolver_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        callFunction(QNS_REGISTRY_CONTRACT_INDEX, 2, input, output);
        return output;
    }

    QNS_REGISTRY::GetTTL_output getTTL(uint64 node) {
        QNS_REGISTRY::GetTTL_input input;
        QNS_REGISTRY::GetTTL_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        callFunction(QNS_REGISTRY_CONTRACT_INDEX, 3, input, output);
        return output;
    }

    QNS_REGISTRY::GetParent_output getParent(uint64 node) {
        QNS_REGISTRY::GetParent_input input;
        QNS_REGISTRY::GetParent_output output;
        memset(&output, 0, sizeof(output));
        input.node = node;
        callFunction(QNS_REGISTRY_CONTRACT_INDEX, 4, input, output);
        return output;
    }
};

// Helper function to create test user IDs
static id createTestId(uint64 value) {
    return m256i(value, 0, 0, 0);
}

// Phase 2: Basic initialization tests
TEST(QNSRegistryTest, ContractInitialization) {
    ContractTestingQNSRegistry test;
    
    // Verify contract state is properly initialized
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1u);  // Root node should exist
    
    // Verify root node exists
    auto nodeExists = test.nodeExists(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(nodeExists.exists);
}

TEST(QNSRegistryTest, RootNodeProperties) {
    ContractTestingQNSRegistry test;
    
    // Check root node owner (owned by contract itself)
    auto ownerResult = test.getOwner(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(ownerResult.exists);
    EXPECT_EQ(ownerResult.owner, id(QNS_REGISTRY_CONTRACT_INDEX, 0, 0, 0));
    
    // Check root node resolver (initially null)
    auto resolverResult = test.getResolver(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(resolverResult.exists);
    EXPECT_EQ(resolverResult.resolver, NULL_ID);
    
    // Check root node TTL (initially 0)
    auto ttlResult = test.getTTL(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(ttlResult.exists);
    EXPECT_EQ(ttlResult.ttl, 0u);
    
    // Check root node parent (root has no parent)
    auto parentResult = test.getParent(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(parentResult.exists);
    EXPECT_EQ(parentResult.parentNode, 0u);
}

TEST(QNSRegistryTest, EpochProcedures) {
    ContractTestingQNSRegistry test;
    
    // Verify initial state
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1u);
    
    // Call epoch procedures
    test.beginEpoch();
    test.endEpoch();
    
    // Verify state is preserved after epoch procedures
    EXPECT_EQ(state->totalNames, 1u);
    
    // Verify root node still exists and functions work
    auto nodeExists = test.nodeExists(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(nodeExists.exists);
}
