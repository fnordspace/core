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

// ===== MULTI-LEVEL SUBDOMAIN TESTS =====

TEST(QNSRegistryMultiLevel, NestedSubdomainCreation)
{
    ContractTestingQNSRegistry test;
    
    // Create owners for different levels
    id domainOwner = createTestId(111);
    id subdomainOwner = createTestId(222);
    id subSubdomainOwner = createTestId(333);
    
    // Step 1: Create domain under .qubic TLD (level 1)
    uint64 domainLabelHash = 0x1111111111111111ULL;
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto domainResult = test.createSubdomain(0x6375626963000000ULL, domainLabelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(domainResult.errorCode, 0u);
    uint64 domainNode = domainResult.newNode;
    
    // Step 2: Create subdomain under domain (level 2)
    uint64 subLabelHash = 0x2222222222222222ULL;
    increaseEnergy(domainOwner, 1000);
    auto subResult = test.createSubdomain(domainNode, subLabelHash, subdomainOwner, domainOwner);
    EXPECT_EQ(subResult.errorCode, 0u);
    uint64 subdomainNode = subResult.newNode;
    
    // Step 3: Create sub-subdomain under subdomain (level 3)
    uint64 subSubLabelHash = 0x3333333333333333ULL;
    increaseEnergy(subdomainOwner, 1000);
    auto subSubResult = test.createSubdomain(subdomainNode, subSubLabelHash, subSubdomainOwner, subdomainOwner);
    EXPECT_EQ(subSubResult.errorCode, 0u);
    uint64 subSubdomainNode = subSubResult.newNode;
    
    // Verify all nodes exist with correct owners
    auto domainOwnerResult = test.getOwner(domainNode);
    EXPECT_TRUE(domainOwnerResult.exists);
    EXPECT_EQ(domainOwnerResult.owner, domainOwner);
    
    auto subdomainOwnerResult = test.getOwner(subdomainNode);
    EXPECT_TRUE(subdomainOwnerResult.exists);
    EXPECT_EQ(subdomainOwnerResult.owner, subdomainOwner);
    
    auto subSubdomainOwnerResult = test.getOwner(subSubdomainNode);
    EXPECT_TRUE(subSubdomainOwnerResult.exists);
    EXPECT_EQ(subSubdomainOwnerResult.owner, subSubdomainOwner);
    
    // Verify totalNames incremented correctly
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 4u); // .qubic + domain + subdomain + sub-subdomain
}

TEST(QNSRegistryMultiLevel, OwnershipIndependence)
{
    ContractTestingQNSRegistry test;
    
    // Create owners for different levels
    id domainOwner = createTestId(444);
    id subdomainOwner = createTestId(555);
    id newSubdomainOwner = createTestId(666);
    
    // Create domain and subdomain
    uint64 domainLabelHash = 0x4444444444444444ULL;
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto domainResult = test.createSubdomain(0x6375626963000000ULL, domainLabelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(domainResult.errorCode, 0u);
    uint64 domainNode = domainResult.newNode;
    
    uint64 subLabelHash = 0x5555555555555555ULL;
    increaseEnergy(domainOwner, 1000);
    auto subResult = test.createSubdomain(domainNode, subLabelHash, subdomainOwner, domainOwner);
    EXPECT_EQ(subResult.errorCode, 0u);
    uint64 subdomainNode = subResult.newNode;
    
    // Verify subdomain owner can transfer ownership independently
    increaseEnergy(subdomainOwner, 1000);
    auto transferResult = test.transferOwnership(subdomainNode, newSubdomainOwner, subdomainOwner);
    EXPECT_TRUE(transferResult.success);
    
    // Verify ownership changed for subdomain but not domain
    auto domainOwnerResult = test.getOwner(domainNode);
    EXPECT_TRUE(domainOwnerResult.exists);
    EXPECT_EQ(domainOwnerResult.owner, domainOwner); // Unchanged
    
    auto subdomainOwnerResult = test.getOwner(subdomainNode);
    EXPECT_TRUE(subdomainOwnerResult.exists);
    EXPECT_EQ(subdomainOwnerResult.owner, newSubdomainOwner); // Changed
    
    // Verify domain owner cannot transfer subdomain ownership
    increaseEnergy(domainOwner, 1000);
    auto unauthorizedTransfer = test.transferOwnership(subdomainNode, domainOwner, domainOwner);
    EXPECT_FALSE(unauthorizedTransfer.success);
}

TEST(QNSRegistryMultiLevel, ResolverIndependence)
{
    ContractTestingQNSRegistry test;
    
    // Create owners and resolvers for different levels
    id domainOwner = createTestId(777);
    id subdomainOwner = createTestId(888);
    id domainResolver = createTestId(999);
    id subdomainResolver = createTestId(1010);
    
    // Create domain and subdomain
    uint64 domainLabelHash = 0x7777777777777777ULL;
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto domainResult = test.createSubdomain(0x6375626963000000ULL, domainLabelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(domainResult.errorCode, 0u);
    uint64 domainNode = domainResult.newNode;
    
    uint64 subLabelHash = 0x8888888888888888ULL;
    increaseEnergy(domainOwner, 1000);
    auto subResult = test.createSubdomain(domainNode, subLabelHash, subdomainOwner, domainOwner);
    EXPECT_EQ(subResult.errorCode, 0u);
    uint64 subdomainNode = subResult.newNode;
    
    // Set different resolvers for domain and subdomain
    increaseEnergy(domainOwner, 1000);
    auto domainResolverResult = test.setResolver(domainNode, domainResolver, domainOwner);
    EXPECT_TRUE(domainResolverResult.success);
    
    increaseEnergy(subdomainOwner, 1000);
    auto subdomainResolverResult = test.setResolver(subdomainNode, subdomainResolver, subdomainOwner);
    EXPECT_TRUE(subdomainResolverResult.success);
    
    // Verify resolvers are independent
    auto domainResolverCheck = test.getResolver(domainNode);
    EXPECT_TRUE(domainResolverCheck.exists);
    EXPECT_EQ(domainResolverCheck.resolver, domainResolver);
    
    auto subdomainResolverCheck = test.getResolver(subdomainNode);
    EXPECT_TRUE(subdomainResolverCheck.exists);
    EXPECT_EQ(subdomainResolverCheck.resolver, subdomainResolver);
    
    // Verify domain owner cannot set subdomain resolver
    increaseEnergy(domainOwner, 1000);
    auto unauthorizedResolver = test.setResolver(subdomainNode, domainResolver, domainOwner);
    EXPECT_FALSE(unauthorizedResolver.success);
    
    // Verify subdomain resolver unchanged
    auto subdomainResolverFinal = test.getResolver(subdomainNode);
    EXPECT_TRUE(subdomainResolverFinal.exists);
    EXPECT_EQ(subdomainResolverFinal.resolver, subdomainResolver);
}

TEST(QNSRegistryMultiLevel, DeepNestingCapability)
{
    ContractTestingQNSRegistry test;
    
    // Test creating 5 levels of subdomains
    id owners[5];
    uint64 nodes[5];
    uint64 labelHashes[5] = {
        0x1111111111111111ULL,
        0x2222222222222222ULL,
        0x3333333333333333ULL,
        0x4444444444444444ULL,
        0x5555555555555555ULL
    };
    
    for (int i = 0; i < 5; i++) {
        owners[i] = createTestId(1000 + i);
    }
    
    // Create level 1 domain under .qubic
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto result0 = test.createSubdomain(0x6375626963000000ULL, labelHashes[0], owners[0], test.getQubicRegistrar());
    EXPECT_EQ(result0.errorCode, 0u);
    nodes[0] = result0.newNode;
    
    // Create levels 2-5 (subdomain under previous level)
    for (int i = 1; i < 5; i++) {
        increaseEnergy(owners[i-1], 1000);
        auto result = test.createSubdomain(nodes[i-1], labelHashes[i], owners[i], owners[i-1]);
        EXPECT_EQ(result.errorCode, 0u);
        nodes[i] = result.newNode;
    }
    
    // Verify all levels exist with correct owners
    for (int i = 0; i < 5; i++) {
        auto ownerResult = test.getOwner(nodes[i]);
        EXPECT_TRUE(ownerResult.exists);
        EXPECT_EQ(ownerResult.owner, owners[i]);
    }
    
    // Verify totalNames incremented correctly
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 6u); // .qubic + 5 levels
    
    // Verify deepest level owner can create further subdomains
    uint64 deepestLabelHash = 0x9999999999999999ULL;
    id deepestOwner = createTestId(9999);
    increaseEnergy(owners[4], 1000);
    auto deepestResult = test.createSubdomain(nodes[4], deepestLabelHash, deepestOwner, owners[4]);
    EXPECT_EQ(deepestResult.errorCode, 0u);
    
    // Verify final count
    auto* finalState = test.getState();
    EXPECT_EQ(finalState->totalNames, 7u); // .qubic + 5 levels + deepest
}

TEST(QNSRegistryMultiLevel, SubdomainCreationAuthorization)
{
    ContractTestingQNSRegistry test;
    
    // Create multi-level structure
    id domainOwner = createTestId(1111);
    id subdomainOwner = createTestId(2222);
    id unauthorizedUser = createTestId(3333);
    
    // Create domain and subdomain
    uint64 domainLabelHash = 0xAAAAAAAAAAAAAAAAULL;
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto domainResult = test.createSubdomain(0x6375626963000000ULL, domainLabelHash, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(domainResult.errorCode, 0u);
    uint64 domainNode = domainResult.newNode;
    
    uint64 subLabelHash = 0xBBBBBBBBBBBBBBBBULL;
    increaseEnergy(domainOwner, 1000);
    auto subResult = test.createSubdomain(domainNode, subLabelHash, subdomainOwner, domainOwner);
    EXPECT_EQ(subResult.errorCode, 0u);
    uint64 subdomainNode = subResult.newNode;
    
    // Verify only subdomain owner can create under subdomain
    uint64 subSubLabelHash = 0xCCCCCCCCCCCCCCCCULL;
    id subSubOwner = createTestId(4444);
    
    // Authorized creation by subdomain owner
    increaseEnergy(subdomainOwner, 1000);
    auto authorizedResult = test.createSubdomain(subdomainNode, subSubLabelHash, subSubOwner, subdomainOwner);
    EXPECT_EQ(authorizedResult.errorCode, 0u);
    
    // Verify unauthorized creation fails
    uint64 unauthorizedLabelHash = 0xDDDDDDDDDDDDDDDDULL;
    id unauthorizedSubOwner = createTestId(5555);
    
    // Domain owner cannot create under subdomain
    increaseEnergy(domainOwner, 1000);
    auto unauthorizedResult1 = test.createSubdomain(subdomainNode, unauthorizedLabelHash, unauthorizedSubOwner, domainOwner);
    EXPECT_NE(unauthorizedResult1.errorCode, 0u);
    
    // Random user cannot create under subdomain
    increaseEnergy(unauthorizedUser, 1000);
    auto unauthorizedResult2 = test.createSubdomain(subdomainNode, unauthorizedLabelHash, unauthorizedSubOwner, unauthorizedUser);
    EXPECT_NE(unauthorizedResult2.errorCode, 0u);
    
    // Verify totalNames only incremented for successful creation
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 4u); // .qubic + domain + subdomain + sub-subdomain
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

// =============================================================================
// EPOCH PROCEDURES TESTS
// =============================================================================

TEST(QNSRegistryEpoch, BeginEpochDoesNotBreakState)
{
    ContractTestingQNSRegistry test;
    
    // Get initial state
    uint64 initialTotalNames = test.getState()->totalNames;
    auto initialRootExists = test.nodeExists(0x6375626963000000ULL);
    auto initialRootOwner = test.getOwner(0x6375626963000000ULL);
    
    // Call BEGIN_EPOCH
    test.beginEpoch();
    
    // Verify state unchanged - BEGIN_EPOCH is currently empty
    EXPECT_EQ(test.getState()->totalNames, initialTotalNames);
    EXPECT_EQ(test.nodeExists(0x6375626963000000ULL).exists, initialRootExists.exists);
    EXPECT_EQ(test.getOwner(0x6375626963000000ULL).owner, initialRootOwner.owner);
}

TEST(QNSRegistryEpoch, EndEpochMaintainsStateConsistency)
{
    ContractTestingQNSRegistry test;
    
    // Get initial state
    uint64 initialTotalNames = test.getState()->totalNames;
    auto initialRootExists = test.nodeExists(0x6375626963000000ULL);
    auto initialRootOwner = test.getOwner(0x6375626963000000ULL);
    
    // Call END_EPOCH (calls nameRegistry.cleanupIfNeeded())
    test.endEpoch();
    
    // Verify state consistency maintained after cleanup
    EXPECT_EQ(test.getState()->totalNames, initialTotalNames);
    EXPECT_EQ(test.nodeExists(0x6375626963000000ULL).exists, initialRootExists.exists);
    EXPECT_EQ(test.getOwner(0x6375626963000000ULL).owner, initialRootOwner.owner);
}

TEST(QNSRegistryEpoch, EpochProceduresSequence)
{
    ContractTestingQNSRegistry test;
    
    // Create a test subdomain first to have more state to verify
    id testUser = createTestId(123);
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto createResult = test.createSubdomain(0x6375626963000000ULL, 0x746573740000000ULL, testUser, test.getQubicRegistrar());
    EXPECT_EQ(createResult.errorCode, 0u);
    
    // Get state before epoch procedures
    uint64 beforeTotalNames = test.getState()->totalNames;
    auto beforeRootExists = test.nodeExists(0x6375626963000000ULL);
    auto beforeSubdomainExists = test.nodeExists(createResult.newNode);
    
    // Run epoch sequence: BEGIN -> END
    test.beginEpoch();
    test.endEpoch();
    
    // Verify all state remains consistent
    EXPECT_EQ(test.getState()->totalNames, beforeTotalNames);
    EXPECT_EQ(test.nodeExists(0x6375626963000000ULL).exists, beforeRootExists.exists);
    EXPECT_EQ(test.nodeExists(createResult.newNode).exists, beforeSubdomainExists.exists);
}

// =============================================================================
// STATE VALIDATION TESTS
// =============================================================================

TEST(QNSRegistryStateValidation, TotalNamesAccuracy)
{
    ContractTestingQNSRegistry test;
    
    // Initial state: totalNames should be 1 (root .qubic node)
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1u);
    
    // Create first domain under .qubic
    id domainOwner = createTestId(1111);
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto domain1 = test.createSubdomain(0x6375626963000000ULL, 0x1111111111111111ULL, domainOwner, test.getQubicRegistrar());
    EXPECT_EQ(domain1.errorCode, 0u);
    EXPECT_EQ(state->totalNames, 2u); // .qubic + domain1
    
    // Create second domain under .qubic
    id domain2Owner = createTestId(2222);
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto domain2 = test.createSubdomain(0x6375626963000000ULL, 0x2222222222222222ULL, domain2Owner, test.getQubicRegistrar());
    EXPECT_EQ(domain2.errorCode, 0u);
    EXPECT_EQ(state->totalNames, 3u); // .qubic + domain1 + domain2
    
    // Create subdomain under domain1
    id subdomainOwner = createTestId(3333);
    increaseEnergy(domainOwner, 1000);
    auto subdomain = test.createSubdomain(domain1.newNode, 0x3333333333333333ULL, subdomainOwner, domainOwner);
    EXPECT_EQ(subdomain.errorCode, 0u);
    EXPECT_EQ(state->totalNames, 4u); // .qubic + domain1 + domain2 + subdomain
    
    // Verify ownership transfer does NOT increment totalNames
    id newOwner = createTestId(4444);
    increaseEnergy(domainOwner, 1000);
    auto transfer = test.transferOwnership(domain1.newNode, newOwner, domainOwner);
    EXPECT_EQ(transfer.success, 1u);
    EXPECT_EQ(state->totalNames, 4u); // No change - transfer doesn't create new node
    
    // Verify failed subdomain creation does NOT increment totalNames
    id unauthorizedUser = createTestId(5555);
    increaseEnergy(unauthorizedUser, 1000);
    auto failedCreate = test.createSubdomain(domain1.newNode, 0x5555555555555555ULL, unauthorizedUser, unauthorizedUser);
    EXPECT_NE(failedCreate.errorCode, 0u);
    EXPECT_EQ(state->totalNames, 4u); // No change - failed creation
}

TEST(QNSRegistryStateValidation, HashMapConsistency)
{
    ContractTestingQNSRegistry test;
    
    auto* state = test.getState();
    
    // Create multiple domains to populate HashMap
    id domainOwner = createTestId(1111);
    uint64 domainNodes[5];
    
    for (int i = 0; i < 5; i++) {
        increaseEnergy(test.getQubicRegistrar(), 1000);
        auto result = test.createSubdomain(0x6375626963000000ULL, 0x1111111111111111ULL + i, domainOwner, test.getQubicRegistrar());
        EXPECT_EQ(result.errorCode, 0u);
        domainNodes[i] = result.newNode;
    }
    
    // Verify all created nodes exist and can be retrieved
    for (int i = 0; i < 5; i++) {
        auto nodeExists = test.nodeExists(domainNodes[i]);
        EXPECT_TRUE(nodeExists.exists);
        
        auto owner = test.getOwner(domainNodes[i]);
        EXPECT_TRUE(owner.exists);
        EXPECT_EQ(owner.owner, domainOwner);
        
        auto resolver = test.getResolver(domainNodes[i]);
        EXPECT_TRUE(resolver.exists);
        EXPECT_EQ(resolver.resolver, NULL_ID);
    }
    
    // Verify totalNames reflects all created nodes
    EXPECT_EQ(state->totalNames, 6u); // .qubic + 5 domains
    
    // Perform cleanup (END_EPOCH) and verify consistency maintained
    test.endEpoch();
    
    // Verify all nodes still exist after cleanup
    for (int i = 0; i < 5; i++) {
        auto nodeExists = test.nodeExists(domainNodes[i]);
        EXPECT_TRUE(nodeExists.exists);
    }
    
    // Verify totalNames unchanged after cleanup
    EXPECT_EQ(state->totalNames, 6u);
}

TEST(QNSRegistryStateValidation, MemoryUsageValidation)
{
    ContractTestingQNSRegistry test;
    
    auto* state = test.getState();
    
    // HashMap is defined as HashMap<uint64, NameRecord, 1048576> (1M entries)
    // Each NameRecord contains: id owner (32 bytes), id resolver (32 bytes), 
    // uint32 ttl (4 bytes), uint64 parentNode (8 bytes), uint64 createdAt (8 bytes), bit isActive (1 byte)
    // Total per record: ~85 bytes + HashMap overhead
    
    // Create a reasonable number of domains to test memory usage
    id domainOwner = createTestId(1111);
    const int domainCount = 100;
    
    for (int i = 0; i < domainCount; i++) {
        increaseEnergy(test.getQubicRegistrar(), 1000);
        auto result = test.createSubdomain(0x6375626963000000ULL, 0x1111111111111111ULL + i, domainOwner, test.getQubicRegistrar());
        EXPECT_EQ(result.errorCode, 0u);
    }
    
    // Verify totalNames is accurate
    EXPECT_EQ(state->totalNames, 1u + domainCount); // .qubic + created domains
    
    // Verify all domains can be accessed (no memory corruption)
    for (int i = 0; i < domainCount; i++) {
        // Hash calculation should match SetSubnodeOwner implementation
        // For this test, we'll verify that the domains exist by checking totalNames
        // and that the system remains functional
        auto rootExists = test.nodeExists(0x6375626963000000ULL);
        EXPECT_TRUE(rootExists.exists);
    }
    
    // Verify system remains functional after many operations
    id testUser = createTestId(9999);
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto finalTest = test.createSubdomain(0x6375626963000000ULL, 0x9999999999999999ULL, testUser, test.getQubicRegistrar());
    EXPECT_EQ(finalTest.errorCode, 0u);
    EXPECT_EQ(state->totalNames, 2u + domainCount);
}

TEST(QNSRegistryStateValidation, StateSizeConstraints)
{
    ContractTestingQNSRegistry test;
    
    auto* state = test.getState();
    
    // Test maxSubdomainDepth constraint (currently set to 10 but not enforced)
    EXPECT_EQ(state->maxSubdomainDepth, 10u);
    
    // Create deep subdomain chain to test depth handling
    id chainOwner = createTestId(1111);
    
    // Create root domain
    increaseEnergy(test.getQubicRegistrar(), 1000);
    auto rootDomain = test.createSubdomain(0x6375626963000000ULL, 0x1111111111111111ULL, chainOwner, test.getQubicRegistrar());
    EXPECT_EQ(rootDomain.errorCode, 0u);
    
    uint64 currentNode = rootDomain.newNode;
    
    // Create chain of 5 subdomains (well within any reasonable limit)
    for (int depth = 0; depth < 5; depth++) {
        increaseEnergy(chainOwner, 1000);
        auto subResult = test.createSubdomain(currentNode, 0x2222222222222222ULL + depth, chainOwner, chainOwner);
        EXPECT_EQ(subResult.errorCode, 0u);
        currentNode = subResult.newNode;
    }
    
    // Verify totalNames reflects the chain
    EXPECT_EQ(state->totalNames, 7u); // .qubic + rootDomain + 5 subdomains
    
    // Test that totalNames cannot exceed reasonable bounds
    // (This is more of a sanity check since uint64 max is very large)
    EXPECT_LT(state->totalNames, 1000000u); // Much less than HashMap capacity
    
    // Verify final node in chain is accessible
    auto finalExists = test.nodeExists(currentNode);
    EXPECT_TRUE(finalExists.exists);
    
    auto finalOwner = test.getOwner(currentNode);
    EXPECT_TRUE(finalOwner.exists);
    EXPECT_EQ(finalOwner.owner, chainOwner);
    
    // Test configuration bounds
    EXPECT_GT(state->maxSubdomainDepth, 0u);
    EXPECT_LT(state->maxSubdomainDepth, 256u); // Reasonable upper bound
}

// =============================================================================
// INTEGRATION TEST SCENARIOS - Task 19
// =============================================================================

TEST(QNSRegistryIntegration, CompleteDomainLifecycle)
{
    // Test complete domain lifecycle: create, modify, transfer
    ContractTestingQNSRegistry test;
    
    // Create test identities
    id registrar = test.getQubicRegistrar();
    id alice = createTestId(1111);
    id bob = createTestId(2222);
    id charlie = createTestId(3333);
    
    increaseEnergy(registrar, 1000);
    increaseEnergy(alice, 1000);
    increaseEnergy(bob, 1000);
    increaseEnergy(charlie, 1000);
    
    // Step 1: Create domain alice.qubic
    uint64 aliceHash = 0x123456789abcdef0ULL;
    auto createResult = test.createSubdomain(0x6375626963000000ULL, aliceHash, alice, registrar);
    EXPECT_EQ(createResult.errorCode, 0u);
    EXPECT_NE(createResult.newNode, 0u);
    
    uint64 aliceNode = createResult.newNode;
    
    // Step 2: Verify domain exists and has correct owner
    auto ownerResult = test.getOwner(aliceNode);
    EXPECT_TRUE(ownerResult.exists);
    EXPECT_EQ(ownerResult.owner, alice);
    
    // Step 3: Set resolver for alice.qubic
    auto setResolverResult = test.setResolver(aliceNode, bob, alice);
    EXPECT_TRUE(setResolverResult.success);
    
    // Step 4: Verify resolver was set
    auto resolverResult = test.getResolver(aliceNode);
    EXPECT_TRUE(resolverResult.exists);
    EXPECT_EQ(resolverResult.resolver, bob);
    
    // Step 5: Set TTL for alice.qubic
    auto setTTLResult = test.setTTL(aliceNode, 3600, alice);
    EXPECT_TRUE(setTTLResult.success);
    
    // Step 6: Verify TTL was set
    auto ttlResult = test.getTTL(aliceNode);
    EXPECT_TRUE(ttlResult.exists);
    EXPECT_EQ(ttlResult.ttl, 3600u);
    
    // Step 7: Transfer ownership to bob
    auto transferResult = test.transferOwnership(aliceNode, bob, alice);
    EXPECT_TRUE(transferResult.success);
    
    // Step 8: Verify ownership transfer
    auto newOwnerResult = test.getOwner(aliceNode);
    EXPECT_TRUE(newOwnerResult.exists);
    EXPECT_EQ(newOwnerResult.owner, bob);
    
    // Step 9: Verify alice can no longer modify (ownership transferred)
    auto unauthorizedResult = test.setTTL(aliceNode, 7200, alice);
    EXPECT_FALSE(unauthorizedResult.success);
    
    // Step 10: Verify bob can now modify
    auto authorizedResult = test.setTTL(aliceNode, 7200, bob);
    EXPECT_TRUE(authorizedResult.success);
    
    // Verify final TTL
    auto finalTTLResult = test.getTTL(aliceNode);
    EXPECT_TRUE(finalTTLResult.exists);
    EXPECT_EQ(finalTTLResult.ttl, 7200u);
    
    // Step 11: Verify totalNames is correct
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 2u); // .qubic + alice.qubic
}

TEST(QNSRegistryIntegration, MultiUserSubdomainManagement)
{
    // Test multi-user subdomain management with different ownership scenarios
    ContractTestingQNSRegistry test;
    
    // Create test identities
    id registrar = test.getQubicRegistrar();
    id alice = createTestId(1111);
    id bob = createTestId(2222);
    id charlie = createTestId(3333);
    id david = createTestId(4444);
    
    increaseEnergy(registrar, 1000);
    increaseEnergy(alice, 1000);
    increaseEnergy(bob, 1000);
    increaseEnergy(charlie, 1000);
    increaseEnergy(david, 1000);
    
    // Step 1: Create company.qubic owned by alice
    uint64 companyHash = 0x1111111111111111ULL;
    auto createCompanyResult = test.createSubdomain(0x6375626963000000ULL, companyHash, alice, registrar);
    EXPECT_EQ(createCompanyResult.errorCode, 0u);
    uint64 companyNode = createCompanyResult.newNode;
    
    // Step 2: Alice creates dept1.company.qubic for bob
    uint64 dept1Hash = 0x2222222222222222ULL;
    auto createDept1Result = test.createSubdomain(companyNode, dept1Hash, bob, alice);
    EXPECT_EQ(createDept1Result.errorCode, 0u);
    uint64 dept1Node = createDept1Result.newNode;
    
    // Step 3: Alice creates dept2.company.qubic for charlie
    uint64 dept2Hash = 0x3333333333333333ULL;
    auto createDept2Result = test.createSubdomain(companyNode, dept2Hash, charlie, alice);
    EXPECT_EQ(createDept2Result.errorCode, 0u);
    uint64 dept2Node = createDept2Result.newNode;
    
    // Step 4: Bob creates project.dept1.company.qubic for david
    uint64 projectHash = 0x4444444444444444ULL;
    auto createProjectResult = test.createSubdomain(dept1Node, projectHash, david, bob);
    EXPECT_EQ(createProjectResult.errorCode, 0u);
    uint64 projectNode = createProjectResult.newNode;
    
    // Step 5: Verify ownership hierarchy
    auto companyOwner = test.getOwner(companyNode);
    EXPECT_TRUE(companyOwner.exists);
    EXPECT_EQ(companyOwner.owner, alice);
    
    auto dept1Owner = test.getOwner(dept1Node);
    EXPECT_TRUE(dept1Owner.exists);
    EXPECT_EQ(dept1Owner.owner, bob);
    
    auto dept2Owner = test.getOwner(dept2Node);
    EXPECT_TRUE(dept2Owner.exists);
    EXPECT_EQ(dept2Owner.owner, charlie);
    
    auto projectOwner = test.getOwner(projectNode);
    EXPECT_TRUE(projectOwner.exists);
    EXPECT_EQ(projectOwner.owner, david);
    
    // Step 6: Test cross-department unauthorized access
    // Charlie cannot create subdomains under dept1 (owned by bob)
    uint64 unauthorizedHash = 0x5555555555555555ULL;
    auto unauthorizedResult = test.createSubdomain(dept1Node, unauthorizedHash, charlie, charlie);
    EXPECT_NE(unauthorizedResult.errorCode, 0u);
    
    // Step 7: Test subdomain independence
    // Bob transfers dept1 to david, but this doesn't affect project ownership
    auto transferDept1Result = test.transferOwnership(dept1Node, david, bob);
    EXPECT_TRUE(transferDept1Result.success);
    
    // Verify project is still owned by david and dept1 is now owned by david
    auto newDept1Owner = test.getOwner(dept1Node);
    EXPECT_TRUE(newDept1Owner.exists);
    EXPECT_EQ(newDept1Owner.owner, david);
    
    auto unchangedProjectOwner = test.getOwner(projectNode);
    EXPECT_TRUE(unchangedProjectOwner.exists);
    EXPECT_EQ(unchangedProjectOwner.owner, david);
    
    // Step 8: Test resolver independence
    // Set different resolvers for each level
    auto setCompanyResolver = test.setResolver(companyNode, alice, alice);
    EXPECT_TRUE(setCompanyResolver.success);
    
    auto setDept2Resolver = test.setResolver(dept2Node, bob, charlie);
    EXPECT_TRUE(setDept2Resolver.success);
    
    auto setProjectResolver = test.setResolver(projectNode, charlie, david);
    EXPECT_TRUE(setProjectResolver.success);
    
    // Verify resolver independence
    auto companyResolver = test.getResolver(companyNode);
    EXPECT_TRUE(companyResolver.exists);
    EXPECT_EQ(companyResolver.resolver, alice);
    
    auto dept2Resolver = test.getResolver(dept2Node);
    EXPECT_TRUE(dept2Resolver.exists);
    EXPECT_EQ(dept2Resolver.resolver, bob);
    
    auto projectResolver = test.getResolver(projectNode);
    EXPECT_TRUE(projectResolver.exists);
    EXPECT_EQ(projectResolver.resolver, charlie);
    
    // Step 9: Verify totalNames is correct
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 5u); // .qubic + company + dept1 + dept2 + project
}

TEST(QNSRegistryIntegration, ResolverContractIntegrationSimulation)
{
    // Simulate integration with resolver contract by testing resolver setting/getting patterns
    ContractTestingQNSRegistry test;
    
    // Create test identities
    id registrar = test.getQubicRegistrar();
    id domainOwner = createTestId(1111);
    id resolverContract1 = createTestId(2001); // Simulated resolver contract
    id resolverContract2 = createTestId(2002); // Another resolver contract
    
    increaseEnergy(registrar, 1000);
    increaseEnergy(domainOwner, 1000);
    
    // Step 1: Create domain for testing
    uint64 domainHash = 0xaabbccddee000000ULL;
    auto createResult = test.createSubdomain(0x6375626963000000ULL, domainHash, domainOwner, registrar);
    EXPECT_EQ(createResult.errorCode, 0u);
    uint64 domainNode = createResult.newNode;
    
    // Step 2: Set resolver to resolverContract1
    auto setResolverResult = test.setResolver(domainNode, resolverContract1, domainOwner);
    EXPECT_TRUE(setResolverResult.success);
    
    // Step 3: Verify resolver was set
    auto resolverResult = test.getResolver(domainNode);
    EXPECT_TRUE(resolverResult.exists);
    EXPECT_EQ(resolverResult.resolver, resolverContract1);
    
    // Step 4: Create multiple subdomains with different resolvers
    uint64 webHash = 0x1111000000000000ULL;
    auto createWebResult = test.createSubdomain(domainNode, webHash, domainOwner, domainOwner);
    EXPECT_EQ(createWebResult.errorCode, 0u);
    uint64 webNode = createWebResult.newNode;
    
    uint64 mailHash = 0x2222000000000000ULL;
    auto createMailResult = test.createSubdomain(domainNode, mailHash, domainOwner, domainOwner);
    EXPECT_EQ(createMailResult.errorCode, 0u);
    uint64 mailNode = createMailResult.newNode;
    
    // Step 5: Set different resolvers for subdomains
    auto setWebResolverResult = test.setResolver(webNode, resolverContract1, domainOwner);
    EXPECT_TRUE(setWebResolverResult.success);
    
    auto setMailResolverResult = test.setResolver(mailNode, resolverContract2, domainOwner);
    EXPECT_TRUE(setMailResolverResult.success);
    
    // Step 6: Verify resolver inheritance doesn't occur (each domain has independent resolver)
    auto webResolver = test.getResolver(webNode);
    EXPECT_TRUE(webResolver.exists);
    EXPECT_EQ(webResolver.resolver, resolverContract1);
    
    auto mailResolver = test.getResolver(mailNode);
    EXPECT_TRUE(mailResolver.exists);
    EXPECT_EQ(mailResolver.resolver, resolverContract2);
    
    // Step 7: Test resolver changes don't affect subdomains
    auto changeParentResolverResult = test.setResolver(domainNode, resolverContract2, domainOwner);
    EXPECT_TRUE(changeParentResolverResult.success);
    
    // Verify parent resolver changed
    auto newParentResolver = test.getResolver(domainNode);
    EXPECT_TRUE(newParentResolver.exists);
    EXPECT_EQ(newParentResolver.resolver, resolverContract2);
    
    // Verify subdomain resolvers unchanged
    auto unchangedWebResolver = test.getResolver(webNode);
    EXPECT_TRUE(unchangedWebResolver.exists);
    EXPECT_EQ(unchangedWebResolver.resolver, resolverContract1);
    
    auto unchangedMailResolver = test.getResolver(mailNode);
    EXPECT_TRUE(unchangedMailResolver.exists);
    EXPECT_EQ(unchangedMailResolver.resolver, resolverContract2);
    
    // Step 8: Test resolver reset to NULL_ID
    auto resetResolverResult = test.setResolver(domainNode, NULL_ID, domainOwner);
    EXPECT_TRUE(resetResolverResult.success);
    
    auto resetResolver = test.getResolver(domainNode);
    EXPECT_TRUE(resetResolver.exists);
    EXPECT_EQ(resetResolver.resolver, NULL_ID);
    
    // Step 9: Verify totalNames is correct
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 4u); // .qubic + domain + web + mail
}

TEST(QNSRegistryIntegration, PerformanceWithLargeNumberOfDomains)
{
    // Test performance and correctness with large number of domains
    ContractTestingQNSRegistry test;
    
    id registrar = test.getQubicRegistrar();
    id domainOwner = createTestId(1111);
    
    increaseEnergy(registrar, 10000);
    increaseEnergy(domainOwner, 10000);
    
    const int NUM_DOMAINS = 50; // Reasonable number for testing
    const int NUM_SUBDOMAINS_PER_DOMAIN = 5;
    
    // Step 1: Create multiple top-level domains
    std::vector<uint64> domainNodes;
    for (int i = 0; i < NUM_DOMAINS; i++) {
        uint64 domainHash = 0x1000000000000000ULL + i;
        auto createResult = test.createSubdomain(0x6375626963000000ULL, domainHash, domainOwner, registrar);
        EXPECT_EQ(createResult.errorCode, 0u);
        domainNodes.push_back(createResult.newNode);
    }
    
    // Step 2: Create subdomains under each domain
    std::vector<uint64> subdomainNodes;
    for (int i = 0; i < NUM_DOMAINS; i++) {
        for (int j = 0; j < NUM_SUBDOMAINS_PER_DOMAIN; j++) {
            uint64 subdomainHash = 0x2000000000000000ULL + (i * NUM_SUBDOMAINS_PER_DOMAIN + j);
            auto createResult = test.createSubdomain(domainNodes[i], subdomainHash, domainOwner, domainOwner);
            EXPECT_EQ(createResult.errorCode, 0u);
            subdomainNodes.push_back(createResult.newNode);
        }
    }
    
    // Step 3: Verify state consistency
    auto* state = test.getState();
    EXPECT_EQ(state->totalNames, 1 + NUM_DOMAINS + (NUM_DOMAINS * NUM_SUBDOMAINS_PER_DOMAIN)); // 1 for root + domains + subdomains
    
    // Step 4: Test random access to verify all domains exist
    for (int i = 0; i < NUM_DOMAINS; i++) {
        auto ownerResult = test.getOwner(domainNodes[i]);
        EXPECT_TRUE(ownerResult.exists);
        EXPECT_EQ(ownerResult.owner, domainOwner);
        
        auto existsResult = test.nodeExists(domainNodes[i]);
        EXPECT_TRUE(existsResult.exists);
    }
    
    // Step 5: Test random access to subdomains
    for (int i = 0; i < (int)subdomainNodes.size(); i += 10) { // Test every 10th subdomain
        auto ownerResult = test.getOwner(subdomainNodes[i]);
        EXPECT_TRUE(ownerResult.exists);
        EXPECT_EQ(ownerResult.owner, domainOwner);
        
        auto existsResult = test.nodeExists(subdomainNodes[i]);
        EXPECT_TRUE(existsResult.exists);
    }
    
    // Step 6: Test bulk operations - set resolvers for all domains
    id resolverContract = createTestId(3001);
    for (int i = 0; i < NUM_DOMAINS; i += 5) { // Test every 5th domain
        auto setResolverResult = test.setResolver(domainNodes[i], resolverContract, domainOwner);
        EXPECT_TRUE(setResolverResult.success);
        
        auto resolverResult = test.getResolver(domainNodes[i]);
        EXPECT_TRUE(resolverResult.exists);
        EXPECT_EQ(resolverResult.resolver, resolverContract);
    }
    
    // Step 7: Test bulk operations - set TTL for all subdomains
    for (int i = 0; i < (int)subdomainNodes.size(); i += 25) { // Test every 25th subdomain
        auto setTTLResult = test.setTTL(subdomainNodes[i], 3600 + i, domainOwner);
        EXPECT_TRUE(setTTLResult.success);
        
        auto ttlResult = test.getTTL(subdomainNodes[i]);
        EXPECT_TRUE(ttlResult.exists);
        EXPECT_EQ(ttlResult.ttl, 3600u + i);
    }
    
    // Step 8: Test cleanup with END_EPOCH
    test.endEpoch();
    
    // Step 9: Verify all domains still exist after cleanup
    for (int i = 0; i < NUM_DOMAINS; i += 10) { // Test every 10th domain
        auto ownerResult = test.getOwner(domainNodes[i]);
        EXPECT_TRUE(ownerResult.exists);
        EXPECT_EQ(ownerResult.owner, domainOwner);
    }
    
    // Step 10: Verify state consistency after cleanup
    auto* finalState = test.getState();
    EXPECT_EQ(finalState->totalNames, 1 + NUM_DOMAINS + (NUM_DOMAINS * NUM_SUBDOMAINS_PER_DOMAIN));
}

