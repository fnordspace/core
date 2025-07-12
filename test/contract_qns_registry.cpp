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
        invokeUserProcedure(QNS_REGISTRY_CONTRACT_INDEX, 3, input, output, caller, 10);
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
        callFunction(QNS_REGISTRY_CONTRACT_INDEX, 4, input, output);
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


    // Get the hard-coded registrar ID
    id getQubicRegistrar() {
        return id(14, 0, 0, 0);  // QUBIC_REGISTRAR_ID
    }
};

// Helper function to create test user IDs
static id createTestId(uint64 value) {
    return m256i(value, 0, 0, 0);
}

// Test cases for GetOwner function
TEST(QNSRegistryGetOwner, RootNodeOwnership)
{
    ContractTestingQNSRegistry test;
    
    // Test getting owner of root node (.qubic TLD hash, should exist and be owned by registrar)
    auto result = test.getOwner(0x6375626963000000ULL);  // QUBIC_ROOT_NODE
    EXPECT_TRUE(result.exists);
    EXPECT_EQ(result.owner, test.getQubicRegistrar());
}

TEST(QNSRegistryGetOwner, NonExistentNodes)
{
    ContractTestingQNSRegistry test;
    
    // Test various non-existent node IDs
    auto result1 = test.getOwner(0);  // Node 0 should not exist (root is hash of "qubic")
    EXPECT_FALSE(result1.exists);
    
    auto result2 = test.getOwner(12345);
    EXPECT_FALSE(result2.exists);
    
    auto result3 = test.getOwner(99999);
    EXPECT_FALSE(result3.exists);
}

TEST(QNSRegistryGetOwner, EdgeCases)
{
    ContractTestingQNSRegistry test;
    
    // Test edge case: maximum uint64 value
    auto result1 = test.getOwner(UINT64_MAX);
    EXPECT_FALSE(result1.exists);
    
    // Test edge case: node 1 (should not exist initially)
    auto result2 = test.getOwner(1);
    EXPECT_FALSE(result2.exists);
}

// Test cases for SetSubnodeOwner procedure  
TEST(QNSRegistrySetSubnodeOwner, NoNewTLDsAllowed)
{
    ContractTestingQNSRegistry test;
    id testUser = createTestId(123);
    uint64 labelHash = 0x1234567890ABCDEFULL;
    
    // No one should be able to create new TLDs (parentNode = 0), not even the registrar
    increaseEnergy(test.getQubicRegistrar(), 1000); // Ensure registrar has enough energy
    auto registrarResult = test.createSubdomain(0, labelHash, testUser, test.getQubicRegistrar());
    EXPECT_EQ(registrarResult.errorCode, 2u); // Root domain creation not allowed
    EXPECT_EQ(registrarResult.newNode, 0u);
    
    // Regular users definitely cannot create TLDs
    increaseEnergy(testUser, 1000); // Ensure test user has enough energy
    auto userResult = test.createSubdomain(0, labelHash, testUser, testUser);
    EXPECT_EQ(userResult.errorCode, 2u); // Root domain creation not allowed
    EXPECT_EQ(userResult.newNode, 0u);
    
    // Verify totalNames unchanged (only .qubic TLD exists)
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1u);
}

TEST(QNSRegistrySetSubnodeOwner, RegistrarCreatesDomainUnderQubicTLD)
{
    ContractTestingQNSRegistry test;
    id domainOwner = createTestId(456);
    uint64 labelHash = 0x1111111111111111ULL;
    
    // First verify .qubic TLD exists and is owned by registrar
    auto qubicOwner = test.getOwner(0x6375626963000000ULL);
    EXPECT_TRUE(qubicOwner.exists);
    EXPECT_EQ(qubicOwner.owner, test.getQubicRegistrar());
    
    increaseEnergy(test.getQubicRegistrar(), 1000); // Ensure registrar has enough energy
    // Registrar should be able to create domains under .qubic TLD
    auto result = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(result.errorCode, 0) << "Error code: " << result.errorCode;
    EXPECT_NE(result.newNode, 0u);
    
    // Verify domain created with correct owner
    auto ownerResult = test.getOwner(result.newNode);
    EXPECT_TRUE(ownerResult.exists);
    EXPECT_EQ(ownerResult.owner, domainOwner);
    
    // Verify totalNames incremented
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 2u); // .qubic + new domain
}

TEST(QNSRegistrySetSubnodeOwner, UnauthorizedCannotCreateUnderQubicTLD)
{
    ContractTestingQNSRegistry test;
    id unauthorizedUser = createTestId(999);
    id domainOwner = createTestId(456);
    uint64 labelHash = 0x1111111111111111ULL;
    
    // Non-registrar should not be able to create domains under .qubic TLD
    increaseEnergy(unauthorizedUser, 1000); // Ensure unauthorized user has enough energy
    auto result = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, unauthorizedUser);
    EXPECT_EQ(result.errorCode, 5u); // Authorization failed
    EXPECT_EQ(result.newNode, 0u);
    
    // Verify totalNames unchanged
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1u); // Only .qubic
}

TEST(QNSRegistrySetSubnodeOwner, DomainOwnerCreatesSubdomain)
{
    ContractTestingQNSRegistry test;
    id domainOwner = createTestId(456);
    id subdomainOwner = createTestId(789);
    
    // First create a domain under .qubic by registrar
    uint64 domainLabelHash = 0x1111111111111111ULL;
    increaseEnergy(test.getQubicRegistrar(), 1000); // Ensure registrar has enough energy
    auto domainResult = test.createSubdomain(0x6375626963000000ULL, domainLabelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(domainResult.errorCode, 0u);
    uint64 domainNode = domainResult.newNode;
    
    // Domain owner should be able to create subdomains
    uint64 subLabelHash = 0x2222222222222222ULL;
    increaseEnergy(domainOwner, 1000); // Ensure domain owner has enough energy
    auto subResult = test.createSubdomain(domainNode, subLabelHash, subdomainOwner, domainOwner);
    EXPECT_EQ(subResult.errorCode, 0u);
    EXPECT_NE(subResult.newNode, 0u);
    
    // Verify subdomain created with correct owner
    auto ownerResult = test.getOwner(subResult.newNode);
    EXPECT_TRUE(ownerResult.exists);
    EXPECT_EQ(ownerResult.owner, subdomainOwner);
    
    // Verify totalNames incremented  
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 3u); // .qubic + domain + subdomain
}

TEST(QNSRegistrySetSubnodeOwner, UnauthorizedCannotCreateSubdomain)
{
    ContractTestingQNSRegistry test;
    id domainOwner = createTestId(456);
    id unauthorizedUser = createTestId(999);
    id subdomainOwner = createTestId(789);
    
    // First create a domain under .qubic by registrar
    uint64 domainLabelHash = 0x1111111111111111ULL;
    increaseEnergy(test.getQubicRegistrar(), 1000); // Ensure registrar has enough energy
    auto domainResult = test.createSubdomain(0x6375626963000000ULL, domainLabelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(domainResult.errorCode, 0u);
    uint64 domainNode = domainResult.newNode;
    
    // Unauthorized user should not be able to create subdomains
    uint64 subLabelHash = 0x2222222222222222ULL;
    increaseEnergy(unauthorizedUser, 1000); // Ensure unauthorized user has energy for the call
    auto subResult = test.createSubdomain(domainNode, subLabelHash, subdomainOwner, unauthorizedUser);
    EXPECT_NE(subResult.errorCode, 0u); // Should fail
    EXPECT_EQ(subResult.newNode, 0u);
    
    // Verify totalNames unchanged
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 2u); // .qubic + domain only
}

TEST(QNSRegistrySetSubnodeOwner, NonExistentParentNode)
{
    ContractTestingQNSRegistry test;
    id testUser = createTestId(123);
    uint64 nonExistentParent = 0x9999999999999999ULL;
    uint64 labelHash = 0x1234567890ABCDEFULL;
    
    // Should fail when parent doesn't exist
    increaseEnergy(testUser, 1000); // Ensure test user has enough energy
    auto result = test.createSubdomain(nonExistentParent, labelHash, testUser, testUser);
    EXPECT_NE(result.errorCode, 0u); // Should fail
    EXPECT_EQ(result.newNode, 0u);
    
    // Verify totalNames unchanged
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1u); // Only .qubic
}

// Re-enable this test - should work correctly now
TEST(QNSRegistryGetOwner, ExistingActiveNode)
{
    ContractTestingQNSRegistry test;
    id testUser = createTestId(123);
    
    // Create a domain under .qubic TLD using registrar
    increaseEnergy(test.getQubicRegistrar(), 1000); // Ensure registrar has enough energy
    auto createResult = test.createSubdomain(0x6375626963000000ULL, 0x1234567890ABCDEFULL, testUser, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    // Now test getting owner of the created node
    auto result = test.getOwner(createResult.newNode);
    EXPECT_TRUE(result.exists);
    EXPECT_EQ(result.owner, testUser);
}

