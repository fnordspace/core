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

// Phase 3: Query function tests
TEST(QNSRegistryTest, NodeExistsFunction) {
    ContractTestingQNSRegistry test;
    
    // Test with root node (should exist)
    auto rootExists = test.nodeExists(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(rootExists.exists);
    
    // Test with non-existent nodes
    auto nonExistent1 = test.nodeExists(0x1234567890ABCDEFULL);
    EXPECT_FALSE(nonExistent1.exists);
    
    auto nonExistent2 = test.nodeExists(0xFFFFFFFFFFFFFFFFULL);
    EXPECT_FALSE(nonExistent2.exists);
    
    // Test edge cases
    auto zeroNode = test.nodeExists(0ULL);
    EXPECT_FALSE(zeroNode.exists);
}

TEST(QNSRegistryTest, GetOwnerFunction) {
    ContractTestingQNSRegistry test;
    
    // Test with root node (should be owned by contract)
    auto rootOwner = test.getOwner(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(rootOwner.exists);
    EXPECT_EQ(rootOwner.owner, id(QNS_REGISTRY_CONTRACT_INDEX, 0, 0, 0));
    
    // Test with non-existent nodes
    auto nonExistentOwner1 = test.getOwner(0x1234567890ABCDEFULL);
    EXPECT_FALSE(nonExistentOwner1.exists);
    
    auto nonExistentOwner2 = test.getOwner(0ULL);
    EXPECT_FALSE(nonExistentOwner2.exists);
    
    auto nonExistentOwner3 = test.getOwner(0xFFFFFFFFFFFFFFFFULL);
    EXPECT_FALSE(nonExistentOwner3.exists);
}

TEST(QNSRegistryTest, GetResolverFunction) {
    ContractTestingQNSRegistry test;
    
    // Test with root node (should have NULL_ID resolver initially)
    auto rootResolver = test.getResolver(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(rootResolver.exists);
    EXPECT_EQ(rootResolver.resolver, NULL_ID);
    
    // Test with non-existent nodes
    auto nonExistentResolver = test.getResolver(0x1234567890ABCDEFULL);
    EXPECT_FALSE(nonExistentResolver.exists);
}

TEST(QNSRegistryTest, GetTTLFunction) {
    ContractTestingQNSRegistry test;
    
    // Test with root node (should have 0 TTL initially)
    auto rootTTL = test.getTTL(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(rootTTL.exists);
    EXPECT_EQ(rootTTL.ttl, 0u);
    
    // Test with non-existent nodes
    auto nonExistentTTL = test.getTTL(0x1234567890ABCDEFULL);
    EXPECT_FALSE(nonExistentTTL.exists);
}

TEST(QNSRegistryTest, GetParentFunction) {
    ContractTestingQNSRegistry test;
    
    // Test with root node (should have no parent - 0)
    auto rootParent = test.getParent(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(rootParent.exists);
    EXPECT_EQ(rootParent.parentNode, 0u);
    
    // Test with non-existent nodes
    auto nonExistentParent = test.getParent(0x1234567890ABCDEFULL);
    EXPECT_FALSE(nonExistentParent.exists);
}

// Phase 4: Ownership management tests
TEST(QNSRegistryTest, SetOwnerProcedure) {
    ContractTestingQNSRegistry test;
    
    id contractOwner = id(QNS_REGISTRY_CONTRACT_INDEX, 0, 0, 0);
    id newOwner = createTestId(12345);
    id unauthorizedUser = createTestId(99999);
    
    // Test unauthorized transfer attempt first (should fail)
    auto unauthorizedResult = test.transferOwnership(QNS_REGISTRY::QUBIC_ROOT_NODE, unauthorizedUser, unauthorizedUser);
    EXPECT_FALSE(unauthorizedResult.success);
    
    // Verify ownership unchanged (still contract owner)
    auto originalOwner = test.getOwner(QNS_REGISTRY::QUBIC_ROOT_NODE);
    EXPECT_TRUE(originalOwner.exists);
    EXPECT_EQ(originalOwner.owner, contractOwner);
    
    // Test setting owner on non-existent node (should fail)
    uint64 nonExistentNode = 0x1234567890ABCDEFULL;
    auto nonExistentResult = test.transferOwnership(nonExistentNode, newOwner, contractOwner);
    EXPECT_FALSE(nonExistentResult.success);
}

TEST(QNSRegistryTest, SetOwnerNonExistentNode) {
    ContractTestingQNSRegistry test;
    
    id testUser = createTestId(12345);
    uint64 nonExistentNode = 0x1234567890ABCDEFULL;
    
    // Test setting owner on non-existent node
    auto result = test.transferOwnership(nonExistentNode, testUser, testUser);
    EXPECT_FALSE(result.success);
    
    // Verify node still doesn't exist
    auto nodeExists = test.nodeExists(nonExistentNode);
    EXPECT_FALSE(nodeExists.exists);
}
