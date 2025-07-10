using namespace QPI;

struct QNS_REGISTRY2
{
};

struct QNS_REGISTRY : public ContractBase
{
    struct NameRecord {
        id owner;           // Domain owner
        id resolver;        // Resolver contract ID
        uint32 ttl;         // Time-to-live in epochs
        uint64 parentNode;  // Parent domain (0 for root)
        uint64 createdAt;   // Creation timestamp
        bit isActive;       // Active status
    };
    // Core registry data - simplified without parent-child tracking
    HashMap<uint64, NameRecord, 1048576> nameRegistry;  // 1M entries, using ~100MB of 1GB available
    
    // Configuration
    uint64 totalNames;
    uint32 maxSubdomainDepth;
    

    // Root node for .qubic TLD - will be computed as K12 hash of "qubic"
    static constexpr uint64 QUBIC_ROOT_NODE = 0x6375626963000000ULL; // Placeholder, actual hash computed by client

    // Function input/output structures
    struct GetOwner_input {
        uint64 node;
    };
    struct GetOwner_output {
        id owner;
        bit exists;
    };

    struct GetResolver_input {
        uint64 node;
    };
    struct GetResolver_output {
        id resolver;
        bit exists;
    };

    struct GetTTL_input {
        uint64 node;
    };
    struct GetTTL_output {
        uint32 ttl;
        bit exists;
    };

    struct GetParent_input {
        uint64 node;
    };
    struct GetParent_output {
        uint64 parentNode;
        bit exists;
    };

    struct NodeExists_input {
        uint64 node;
    };
    struct NodeExists_output {
        bit exists;
    };

    // Procedure input/output structures
    struct SetOwner_input {
        uint64 node;
        id newOwner;
    };
    struct SetOwner_output {
        bit success;
    };

    struct SetResolver_input {
        uint64 node;
        id resolver;
    };
    struct SetResolver_output {
        bit success;
    };

    struct SetSubnodeOwner_input {
        uint64 parentNode;
        uint64 labelHash;   // Hash of the subdomain label
        id owner;
    };
    struct SetSubnodeOwner_output {
        uint64 newNode;
        bit success;
    };

    struct SetTTL_input {
        uint64 node;
        uint32 ttl;
    };
    struct SetTTL_output {
        bit success;
    };

    // Locals structs for functions that need local variables
    struct GetOwner_locals {
        NameRecord record;
    };

    struct GetResolver_locals {
        NameRecord record;
    };

    struct GetTTL_locals {
        NameRecord record;
    };

    struct GetParent_locals {
        NameRecord record;
    };

    struct NodeExists_locals {
        NameRecord record;
    };

    // Locals structs for procedures that need local variables
    struct SetOwner_locals {
        NameRecord record;
    };

    struct SetResolver_locals {
        NameRecord record;
    };

    struct SetTTL_locals {
        NameRecord record;
    };

    struct SetSubnodeOwner_locals {
        NameRecord parentRecord;
        NameRecord existingRecord;
        NameRecord newRecord;
        struct NodeData {
            uint64 parentNode;
            uint64 labelHash;
        } nodeData;
    };

    // Registry functions
    PUBLIC_FUNCTION_WITH_LOCALS(GetOwner)
    {
        output.exists = false;
        
        if (state.nameRegistry.contains(input.node)) {
            if (state.nameRegistry.get(input.node, locals.record) && locals.record.isActive) {
                output.owner = locals.record.owner;
                output.exists = true;
            }
        }
    }

    PUBLIC_FUNCTION_WITH_LOCALS(GetResolver)
    {
        output.exists = false;
        
        if (state.nameRegistry.contains(input.node)) {
            if (state.nameRegistry.get(input.node, locals.record) && locals.record.isActive) {
                output.resolver = locals.record.resolver;
                output.exists = true;
            }
        }
    }

    PUBLIC_FUNCTION_WITH_LOCALS(GetTTL)
    {
        output.exists = false;
        
        if (state.nameRegistry.contains(input.node)) {
            if (state.nameRegistry.get(input.node, locals.record) && locals.record.isActive) {
                output.ttl = locals.record.ttl;
                output.exists = true;
            }
        }
    }

    PUBLIC_FUNCTION_WITH_LOCALS(GetParent)
    {
        output.exists = false;
        
        if (state.nameRegistry.contains(input.node)) {
            if (state.nameRegistry.get(input.node, locals.record) && locals.record.isActive) {
                output.parentNode = locals.record.parentNode;
                output.exists = true;
            }
        }
    }

    PUBLIC_FUNCTION_WITH_LOCALS(NodeExists)
    {
        output.exists = false;
        
        if (state.nameRegistry.contains(input.node)) {
            if (state.nameRegistry.get(input.node, locals.record)) {
                output.exists = locals.record.isActive;
            }
        }
    }

    // Registry procedures
    PUBLIC_PROCEDURE_WITH_LOCALS(SetOwner)
    {
        output.success = false;
        
        // Check if node exists
        if (!state.nameRegistry.contains(input.node)) {
            return;
        }
        
        if (!state.nameRegistry.get(input.node, locals.record) || !locals.record.isActive) {
            return;
        }
        
        // Check if caller is current owner
        if (!(qpi.invocator() == locals.record.owner)) {
            return;
        }
        
        // Update owner
        locals.record.owner = input.newOwner;
        
        // Update registry
        state.nameRegistry.set(input.node, locals.record);
        
        output.success = true;
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(SetResolver)
    {
        output.success = false;
        
        // Check if node exists
        if (!state.nameRegistry.contains(input.node)) {
            return;
        }
        
        if (!state.nameRegistry.get(input.node, locals.record) || !locals.record.isActive) {
            return;
        }
        
        // Check if caller is owner
        if (!(qpi.invocator() == locals.record.owner)) {
            return;
        }
        
        // Update resolver
        locals.record.resolver = input.resolver;
        state.nameRegistry.set(input.node, locals.record);
        
        output.success = true;
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(SetSubnodeOwner)
    {
        output.success = false;
        output.newNode = 0;
        
        // Check if parent node exists (or is root)
        if (input.parentNode != 0) {
            if (!state.nameRegistry.contains(input.parentNode)) {
                return;
            }
            
            if (!state.nameRegistry.get(input.parentNode, locals.parentRecord) || !locals.parentRecord.isActive) {
                return;
            }
            
            // Check if caller is parent owner
            if (!(qpi.invocator() == locals.parentRecord.owner)) {
                return;
            }
        }
        
        // Generate subnode hash (client provides parent and label hash, we combine them)
        locals.nodeData.parentNode = input.parentNode;
        locals.nodeData.labelHash = input.labelHash;
        output.newNode = qpi.K12(locals.nodeData).m256i_u64[0];
        
        // Check if subnode already exists
        if (state.nameRegistry.contains(output.newNode)) {
            if (state.nameRegistry.get(output.newNode, locals.existingRecord) && locals.existingRecord.isActive) {
                // Node already exists - transfer ownership only
                locals.existingRecord.owner = input.owner;
                state.nameRegistry.set(output.newNode, locals.existingRecord);
                output.success = true;
                return;
            }
        }
        
        // Create new name record
        locals.newRecord.owner = input.owner;
        locals.newRecord.resolver = NULL_ID;  // No resolver initially
        locals.newRecord.ttl = 0;  // No TTL initially
        locals.newRecord.parentNode = input.parentNode;
        locals.newRecord.createdAt = qpi.tick();
        locals.newRecord.isActive = true;
        
        // Add to registry
        state.nameRegistry.set(output.newNode, locals.newRecord);
        
        // Increment total names
        state.totalNames++;
        
        output.success = true;
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(SetTTL)
    {
        output.success = false;
        
        // Check if node exists
        if (!state.nameRegistry.contains(input.node)) {
            return;
        }
        
        if (!state.nameRegistry.get(input.node, locals.record) || !locals.record.isActive) {
            return;
        }
        
        // Check if caller is owner
        if (!(qpi.invocator() == locals.record.owner)) {
            return;
        }
        
        // Update TTL
        locals.record.ttl = input.ttl;
        state.nameRegistry.set(input.node, locals.record);
        
        output.success = true;
    }

    // System procedures
    INITIALIZE()
    {
        // Initialize state
        state.nameRegistry.reset();
        
        state.totalNames = 0;
        state.maxSubdomainDepth = 10;  // Maximum 10 levels of subdomains
        
        // Create root .qubic node
        NameRecord rootRecord;
        rootRecord.owner = SELF;  // Contract owns the root
        rootRecord.resolver = NULL_ID;
        rootRecord.ttl = 0;
        rootRecord.parentNode = 0;  // Root has no parent
        rootRecord.createdAt = qpi.tick();
        rootRecord.isActive = true;
        
        state.nameRegistry.set(QUBIC_ROOT_NODE, rootRecord);
        state.totalNames = 1;
    }

    BEGIN_EPOCH()
    {
        // Cleanup expired names if needed
        // For now, we don't implement automatic expiration in registry
        // This will be handled by the registrar contract
    }

    END_EPOCH()
    {
        // Perform any necessary cleanup
        state.nameRegistry.cleanupIfNeeded();
    }

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES()
    {
        // Register functions
        REGISTER_USER_FUNCTION(GetOwner, 1);
        REGISTER_USER_FUNCTION(GetResolver, 2);
        REGISTER_USER_FUNCTION(GetTTL, 3);
        REGISTER_USER_FUNCTION(GetParent, 4);
        REGISTER_USER_FUNCTION(NodeExists, 5);
        
        // Register procedures
        REGISTER_USER_PROCEDURE(SetOwner, 1);
        REGISTER_USER_PROCEDURE(SetResolver, 2);
        REGISTER_USER_PROCEDURE(SetSubnodeOwner, 3);
        REGISTER_USER_PROCEDURE(SetTTL, 4);
    }
};
