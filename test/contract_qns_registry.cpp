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

// Test cases for GetResolver query function
TEST(QNSRegistryGetResolver, RootNodeResolver)
{
    ContractTestingQNSRegistry test;
    
    // Test getting resolver of root node (.qubic TLD hash)
    // Initially should exist but have NULL_ID resolver
    auto result = test.getResolver(0x6375626963000000ULL);  // QUBIC_ROOT_NODE
    EXPECT_TRUE(result.exists);
    EXPECT_EQ(result.resolver, NULL_ID);
}

TEST(QNSRegistryGetResolver, NonExistentNodes)
{
    ContractTestingQNSRegistry test;
    
    // Test various non-existent node IDs
    auto result1 = test.getResolver(0);  // Node 0 should not exist
    EXPECT_FALSE(result1.exists);
    
    auto result2 = test.getResolver(12345);
    EXPECT_FALSE(result2.exists);
    
    auto result3 = test.getResolver(99999);
    EXPECT_FALSE(result3.exists);
}

TEST(QNSRegistryGetResolver, EdgeCases)
{
    ContractTestingQNSRegistry test;
    
    // Test edge case: maximum uint64 value
    auto result1 = test.getResolver(UINT64_MAX);
    EXPECT_FALSE(result1.exists);
    
    // Test edge case: node 1 (should not exist initially)
    auto result2 = test.getResolver(1);
    EXPECT_FALSE(result2.exists);
}

// Test cases for GetTTL query function
TEST(QNSRegistryGetTTL, RootNodeTTL)
{
    ContractTestingQNSRegistry test;
    
    // Test root node (.qubic TLD) TTL - should exist with initial TTL=0
    auto result = test.getTTL(0x6375626963000000ULL);
    EXPECT_TRUE(result.exists);
    EXPECT_EQ(result.ttl, 0u); // Initial TTL should be 0
}

TEST(QNSRegistryGetTTL, NonExistentNodes)
{
    ContractTestingQNSRegistry test;
    
    // Test non-existent node
    auto result1 = test.getTTL(0x1111111111111111ULL);
    EXPECT_FALSE(result1.exists);
    
    // Test node 0 (should not exist - root is the hash)
    auto result2 = test.getTTL(0);
    EXPECT_FALSE(result2.exists);
}

TEST(QNSRegistryGetTTL, EdgeCases)
{
    ContractTestingQNSRegistry test;
    
    // Test edge case: maximum uint64 value
    auto result1 = test.getTTL(UINT64_MAX);
    EXPECT_FALSE(result1.exists);
    
    // Test edge case: node 1 (should not exist initially)
    auto result2 = test.getTTL(1);
    EXPECT_FALSE(result2.exists);
}

// Test cases for NodeExists query function
TEST(QNSRegistryNodeExists, RootNodeExists)
{
    ContractTestingQNSRegistry test;
    
    // Root node (.qubic TLD) should exist after initialization
    auto result = test.nodeExists(0x6375626963000000ULL);  // QUBIC_ROOT_NODE
    EXPECT_TRUE(result.exists);
}

TEST(QNSRegistryNodeExists, NonExistentNodes)
{
    ContractTestingQNSRegistry test;
    
    // Test node 0 (should not exist)
    auto result1 = test.nodeExists(0);
    EXPECT_FALSE(result1.exists);
    
    // Test arbitrary non-existent node
    auto result2 = test.nodeExists(12345);
    EXPECT_FALSE(result2.exists);
}

TEST(QNSRegistryNodeExists, EdgeCases)
{
    ContractTestingQNSRegistry test;
    
    // Test edge case: maximum uint64 value
    auto result1 = test.nodeExists(UINT64_MAX);
    EXPECT_FALSE(result1.exists);
    
    // Test edge case: node 1 (should not exist initially)
    auto result2 = test.nodeExists(1);
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

// SetOwner procedure tests
TEST(QNSRegistrySetOwner, SuccessfulOwnershipTransfer)
{
    ContractTestingQNSRegistry test;
    id originalOwner = createTestId(123);
    id newOwner = createTestId(456);
    
    // Create a domain under .qubic TLD using registrar
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, 0x1234567890ABCDEFULL, originalOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    uint64 domainNode = createResult.newNode;
    
    // Verify original ownership
    auto ownerBefore = test.getOwner(domainNode);
    EXPECT_TRUE(ownerBefore.exists);
    EXPECT_EQ(ownerBefore.owner, originalOwner);
    
    // Transfer ownership
    increaseEnergy(originalOwner, 1000);
    auto transferResult = test.transferOwnership(domainNode, newOwner, originalOwner);
    EXPECT_TRUE(transferResult.success);
    
    // Verify ownership changed
    auto ownerAfter = test.getOwner(domainNode);
    EXPECT_TRUE(ownerAfter.exists);
    EXPECT_EQ(ownerAfter.owner, newOwner);
    EXPECT_NE(ownerAfter.owner, originalOwner);
}

TEST(QNSRegistrySetOwner, UnauthorizedTransferAttempt)
{
    ContractTestingQNSRegistry test;
    id originalOwner = createTestId(123);
    id unauthorizedUser = createTestId(789);
    id newOwner = createTestId(456);
    
    // Create a domain under .qubic TLD using registrar
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, 0x1234567890ABCDEFULL, originalOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    uint64 domainNode = createResult.newNode;
    
    // Verify original ownership
    auto ownerBefore = test.getOwner(domainNode);
    EXPECT_TRUE(ownerBefore.exists);
    EXPECT_EQ(ownerBefore.owner, originalOwner);
    
    // Unauthorized user attempts to transfer ownership
    increaseEnergy(unauthorizedUser, 1000);
    auto transferResult = test.transferOwnership(domainNode, newOwner, unauthorizedUser);
    EXPECT_FALSE(transferResult.success);
    
    // Verify ownership unchanged
    auto ownerAfter = test.getOwner(domainNode);
    EXPECT_TRUE(ownerAfter.exists);
    EXPECT_EQ(ownerAfter.owner, originalOwner);
    EXPECT_NE(ownerAfter.owner, newOwner);
}

TEST(QNSRegistrySetOwner, NonExistentNodeTransfer)
{
    ContractTestingQNSRegistry test;
    id caller = createTestId(123);
    id newOwner = createTestId(456);
    uint64 nonExistentNode = 0x9999999999999999ULL;
    
    // Verify node doesn't exist
    auto ownerCheck = test.getOwner(nonExistentNode);
    EXPECT_FALSE(ownerCheck.exists);
    
    // Attempt to transfer ownership of non-existent node
    increaseEnergy(caller, 1000);
    auto transferResult = test.transferOwnership(nonExistentNode, newOwner, caller);
    EXPECT_FALSE(transferResult.success);
}

TEST(QNSRegistrySetOwner, RootNodeOwnershipTransfer)
{
    ContractTestingQNSRegistry test;
    id newOwner = createTestId(456);
    
    // Get current root node owner
    auto rootOwner = test.getOwner(0x6375626963000000ULL);
    EXPECT_TRUE(rootOwner.exists);
    
    // Current root owner attempts to transfer ownership
    increaseEnergy(rootOwner.owner, 1000);
    auto transferResult = test.transferOwnership(0x6375626963000000ULL, newOwner, rootOwner.owner);
    EXPECT_TRUE(transferResult.success);
    
    // Verify root ownership changed
    auto newRootOwner = test.getOwner(0x6375626963000000ULL);
    EXPECT_TRUE(newRootOwner.exists);
    EXPECT_EQ(newRootOwner.owner, newOwner);
}

// ===== SET RESOLVER TESTS =====

TEST(QNSRegistrySetResolver, SuccessfulResolverSetting)
{
    ContractTestingQNSRegistry test;
    
    // Create a test domain first
    id domainOwner = createTestId(555);
    uint64 labelHash = 0x1234567890ABCDEFULL;
    
    // Create domain under .qubic TLD
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    uint64 testNode = createResult.newNode;
    id newResolver = createTestId(999);
    
    // Verify initial resolver is NULL_ID
    auto resolverBefore = test.getResolver(testNode);
    EXPECT_TRUE(resolverBefore.exists);
    EXPECT_EQ(resolverBefore.resolver, NULL_ID);
    
    // Set resolver by owner
    increaseEnergy(domainOwner, 1000);
    auto setResult = test.setResolver(testNode, newResolver, domainOwner);
    EXPECT_TRUE(setResult.success);
    
    // Verify resolver was actually updated
    auto resolverAfter = test.getResolver(testNode);
    EXPECT_TRUE(resolverAfter.exists);
    EXPECT_EQ(resolverAfter.resolver, newResolver);
    EXPECT_NE(resolverAfter.resolver, NULL_ID);
}

TEST(QNSRegistrySetResolver, UnauthorizedResolverSetting)
{
    ContractTestingQNSRegistry test;
    
    // Create a test domain first
    id domainOwner = createTestId(555);
    id unauthorizedUser = createTestId(666);
    uint64 labelHash = 0x1234567890ABCDEFULL;
    
    // Create domain under .qubic TLD
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    uint64 testNode = createResult.newNode;
    id newResolver = createTestId(999);
    
    // Verify initial resolver
    auto resolverBefore = test.getResolver(testNode);
    EXPECT_TRUE(resolverBefore.exists);
    EXPECT_EQ(resolverBefore.resolver, NULL_ID);
    
    // Attempt to set resolver by unauthorized user
    increaseEnergy(unauthorizedUser, 1000);
    auto setResult = test.setResolver(testNode, newResolver, unauthorizedUser);
    EXPECT_FALSE(setResult.success);
    
    // Verify resolver remains unchanged
    auto resolverAfter = test.getResolver(testNode);
    EXPECT_TRUE(resolverAfter.exists);
    EXPECT_EQ(resolverAfter.resolver, NULL_ID);
    EXPECT_NE(resolverAfter.resolver, newResolver);
}

TEST(QNSRegistrySetResolver, NonExistentNodeResolver)
{
    ContractTestingQNSRegistry test;
    id caller = createTestId(123);
    id newResolver = createTestId(999);
    uint64 nonExistentNode = 0x9999999999999999ULL;
    
    // Verify node doesn't exist
    auto resolverCheck = test.getResolver(nonExistentNode);
    EXPECT_FALSE(resolverCheck.exists);
    
    // Attempt to set resolver for non-existent node
    increaseEnergy(caller, 1000);
    auto setResult = test.setResolver(nonExistentNode, newResolver, caller);
    EXPECT_FALSE(setResult.success);
}

TEST(QNSRegistrySetResolver, RootNodeResolverSetting)
{
    ContractTestingQNSRegistry test;
    id newResolver = createTestId(999);
    
    // Get current root node owner
    auto rootOwner = test.getOwner(0x6375626963000000ULL);
    EXPECT_TRUE(rootOwner.exists);
    
    // Verify initial root resolver
    auto resolverBefore = test.getResolver(0x6375626963000000ULL);
    EXPECT_TRUE(resolverBefore.exists);
    EXPECT_EQ(resolverBefore.resolver, NULL_ID);
    
    // Set resolver for root node by owner
    increaseEnergy(rootOwner.owner, 1000);
    auto setResult = test.setResolver(0x6375626963000000ULL, newResolver, rootOwner.owner);
    EXPECT_TRUE(setResult.success);
    
    // Verify root resolver was updated
    auto resolverAfter = test.getResolver(0x6375626963000000ULL);
    EXPECT_TRUE(resolverAfter.exists);
    EXPECT_EQ(resolverAfter.resolver, newResolver);
}

TEST(QNSRegistrySetResolver, NullResolverSetting)
{
    ContractTestingQNSRegistry test;
    
    // Create a test domain and set a resolver first
    id domainOwner = createTestId(555);
    uint64 labelHash = 0x1234567890ABCDEFULL;
    id initialResolver = createTestId(888);
    
    // Create domain under .qubic TLD
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    uint64 testNode = createResult.newNode;
    
    // Set initial resolver
    increaseEnergy(domainOwner, 1000);
    auto setResult1 = test.setResolver(testNode, initialResolver, domainOwner);
    EXPECT_TRUE(setResult1.success);
    
    // Verify resolver is set
    auto resolverMiddle = test.getResolver(testNode);
    EXPECT_TRUE(resolverMiddle.exists);
    EXPECT_EQ(resolverMiddle.resolver, initialResolver);
    
    // Reset resolver to NULL_ID
    increaseEnergy(domainOwner, 1000);
    auto setResult2 = test.setResolver(testNode, NULL_ID, domainOwner);
    EXPECT_TRUE(setResult2.success);
    
    // Verify resolver was reset to NULL_ID
    auto resolverAfter = test.getResolver(testNode);
    EXPECT_TRUE(resolverAfter.exists);
    EXPECT_EQ(resolverAfter.resolver, NULL_ID);
}

// ===== SET TTL TESTS =====

TEST(QNSRegistrySetTTL, SuccessfulTTLSetting)
{
    ContractTestingQNSRegistry test;
    
    // Create a test domain first
    id domainOwner = createTestId(777);
    uint64 labelHash = 0x1234567890ABCDEFULL;
    
    // Create domain under .qubic TLD
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    uint64 testNode = createResult.newNode;
    uint32 newTTL = 3600; // 1 hour
    
    // Verify initial TTL is 0
    auto ttlBefore = test.getTTL(testNode);
    EXPECT_TRUE(ttlBefore.exists);
    EXPECT_EQ(ttlBefore.ttl, 0u);
    
    // Set TTL by owner
    increaseEnergy(domainOwner, 1000);
    auto setResult = test.setTTL(testNode, newTTL, domainOwner);
    EXPECT_TRUE(setResult.success);
    
    // Verify TTL was actually updated
    auto ttlAfter = test.getTTL(testNode);
    EXPECT_TRUE(ttlAfter.exists);
    EXPECT_EQ(ttlAfter.ttl, newTTL);
}

TEST(QNSRegistrySetTTL, UnauthorizedTTLSetting)
{
    ContractTestingQNSRegistry test;
    
    // Create a test domain first
    id domainOwner = createTestId(777);
    id unauthorizedUser = createTestId(888);
    uint64 labelHash = 0x1234567890ABCDEFULL;
    
    // Create domain under .qubic TLD
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    uint64 testNode = createResult.newNode;
    uint32 newTTL = 3600;
    
    // Verify initial TTL is 0
    auto ttlBefore = test.getTTL(testNode);
    EXPECT_TRUE(ttlBefore.exists);
    EXPECT_EQ(ttlBefore.ttl, 0u);
    
    // Attempt to set TTL by unauthorized user
    increaseEnergy(unauthorizedUser, 1000);
    auto setResult = test.setTTL(testNode, newTTL, unauthorizedUser);
    EXPECT_FALSE(setResult.success);
    
    // Verify TTL was not changed
    auto ttlAfter = test.getTTL(testNode);
    EXPECT_TRUE(ttlAfter.exists);
    EXPECT_EQ(ttlAfter.ttl, 0u);
}

TEST(QNSRegistrySetTTL, NonExistentNodeTTL)
{
    ContractTestingQNSRegistry test;
    id testUser = createTestId(999);
    uint64 nonExistentNode = 0x9999999999999999ULL;
    uint32 newTTL = 3600;
    
    // Attempt to set TTL for non-existent node
    increaseEnergy(testUser, 1000);
    auto setResult = test.setTTL(nonExistentNode, newTTL, testUser);
    EXPECT_FALSE(setResult.success);
    
    // Verify node still doesn't exist
    auto ttlCheck = test.getTTL(nonExistentNode);
    EXPECT_FALSE(ttlCheck.exists);
}

TEST(QNSRegistrySetTTL, RootNodeTTLSetting)
{
    ContractTestingQNSRegistry test;
    uint32 newTTL = 86400; // 24 hours
    
    // Get current root node owner
    auto rootOwner = test.getOwner(0x6375626963000000ULL);
    EXPECT_TRUE(rootOwner.exists);
    
    // Verify initial root TTL is 0
    auto ttlBefore = test.getTTL(0x6375626963000000ULL);
    EXPECT_TRUE(ttlBefore.exists);
    EXPECT_EQ(ttlBefore.ttl, 0u);
    
    // Set TTL for root node by owner
    increaseEnergy(rootOwner.owner, 1000);
    auto setResult = test.setTTL(0x6375626963000000ULL, newTTL, rootOwner.owner);
    EXPECT_TRUE(setResult.success);
    
    // Verify root TTL was updated
    auto ttlAfter = test.getTTL(0x6375626963000000ULL);
    EXPECT_TRUE(ttlAfter.exists);
    EXPECT_EQ(ttlAfter.ttl, newTTL);
}

TEST(QNSRegistrySetTTL, ZeroTTLSetting)
{
    ContractTestingQNSRegistry test;
    
    // Create a test domain and set initial TTL
    id domainOwner = createTestId(777);
    uint64 labelHash = 0x1234567890ABCDEFULL;
    
    // Create domain under .qubic TLD
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, labelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    uint64 testNode = createResult.newNode;
    
    // Set initial TTL to non-zero value
    increaseEnergy(domainOwner, 1000);
    auto setResult1 = test.setTTL(testNode, 3600, domainOwner);
    EXPECT_TRUE(setResult1.success);
    
    // Verify TTL is set
    auto ttlMiddle = test.getTTL(testNode);
    EXPECT_TRUE(ttlMiddle.exists);
    EXPECT_EQ(ttlMiddle.ttl, 3600u);
    
    // Reset TTL to zero
    increaseEnergy(domainOwner, 1000);
    auto setResult2 = test.setTTL(testNode, 0, domainOwner);
    EXPECT_TRUE(setResult2.success);
    
    // Verify TTL was reset to zero
    auto ttlAfter = test.getTTL(testNode);
    EXPECT_TRUE(ttlAfter.exists);
    EXPECT_EQ(ttlAfter.ttl, 0u);
}

// ===== INITIALIZE PROCEDURE TESTS =====

TEST(QNSRegistryInitialize, StateInitialization)
{
    ContractTestingQNSRegistry test;
    
    // Verify initial state values after INITIALIZE
    auto state = test.getState();
    
    // Check totalNames is set to 1 (only root .qubic TLD)
    EXPECT_EQ(state->totalNames, 1u);
    
    // Check maxSubdomainDepth is set correctly
    EXPECT_EQ(state->maxSubdomainDepth, 10u);
}

TEST(QNSRegistryInitialize, RootNodeCreation)
{
    ContractTestingQNSRegistry test;
    
    // Verify root .qubic node was created correctly
    auto rootExists = test.nodeExists(0x6375626963000000ULL);  // QUBIC_ROOT_NODE
    EXPECT_TRUE(rootExists.exists);
    
    // Verify root node owner is registrar (ID 14)
    auto rootOwner = test.getOwner(0x6375626963000000ULL);
    EXPECT_TRUE(rootOwner.exists);
    EXPECT_EQ(rootOwner.owner, test.getQubicRegistrar());
    
    // Verify root node resolver is NULL_ID initially
    auto rootResolver = test.getResolver(0x6375626963000000ULL);
    EXPECT_TRUE(rootResolver.exists);
    EXPECT_EQ(rootResolver.resolver, NULL_ID);
    
    // Verify root node TTL is 0 initially
    auto rootTTL = test.getTTL(0x6375626963000000ULL);
    EXPECT_TRUE(rootTTL.exists);
    EXPECT_EQ(rootTTL.ttl, 0u);
}

TEST(QNSRegistryInitialize, RegistryReset)
{
    ContractTestingQNSRegistry test;
    
    // The nameRegistry.reset() call should ensure clean state
    // This is implicitly tested by all other tests working correctly
    
    // Verify only root node exists, no other nodes
    EXPECT_FALSE(test.nodeExists(0).exists);  // Node 0 should not exist
    EXPECT_FALSE(test.nodeExists(1).exists);  // Node 1 should not exist
    EXPECT_FALSE(test.nodeExists(12345).exists);  // Random node should not exist
    
    // Only QUBIC_ROOT_NODE should exist
    EXPECT_TRUE(test.nodeExists(0x6375626963000000ULL).exists);
}

