// #include "contracts/qpi.h"
#include "qpi.h"
using namespace QPI;
// #include "../contract_core/qpi_hash_map_impl.h"
// Qubic Name Service Contract


// constexpr unsigned long long QNS_NAME_LENGTH = 256;

// // Datatype for the Name that is used in QNS e.g. myAwesomeName.qubic
// //using QNSName = uint8[QNS_NAME_LENGTH];
// using QNSOwner = QPI::id;
using IPFSHash = m256i;


// int compareMemory(const void* ptr1, const void* ptr2, unsigned long long num) {
//     const unsigned char* p1 = static_cast<const unsigned char*>(ptr1);
//     const unsigned char* p2 = static_cast<const unsigned char*>(ptr2);
//     for (unsigned long long i = 0; i < num; ++i) {
//         if (p1[i] != p2[i]) {
//             return p1[i] - p2[i];
//         }
//     }
//     return 0;
// }

// struct QNSName {
//   unsigned char name[QNS_NAME_LENGTH];
  
//     bool operator==(const QNSName& other) const {
//         return compareMemory(name, other.name, QNS_NAME_LENGTH) == 0;
//     }
// };


//Structure of each entry behind the lookup.
struct QNSEntry {
  // Name of the entry. Potentially needed to check for hash collisions
  //QNSName name;
  // Id to look up
  QPI::id id;
  // Owner
  QPI::id  owner;
  // ipfs hash
  // IPFSHash ipfs;
  // Expiration date
  //QPI::uint8 expiration;
};

struct QNS2 {};

// Define state of the contract
struct QNS : public ContractBase
{
  /**************************************/
  /************CONTRACT STATES***********/
  /**************************************/

  // Mapping from names to Qubic IDs
  // QPI::collection<QPI::id, 1024> nameToID;
    QPI::HashMap<bit_1024, QNSEntry, 1024> lookupMap;


    struct QNSLogger
    {
        uint32 _contractIndex;
        uint32 _type;
        QPI::id originator;
        QPI::id id;
        QPI::id owner;
        sint8 _terminator;
    };

    /**************************************/
    /********INPUT AND OUTPUT STRUCTS******/
    /**************************************/

    // Register

    struct registerName_input {
        //QNSEntry entry;
        QPI::bit_1024 name;
        QNSEntry entry;
    };

    struct registerName_output {
        QPI::sint32 returnCode;
    };

    struct registerName_locals {
        QNSLogger logger;
    };
    // // Update

    // struct update_input {
    //   QNSEntry entry;
    // };

    // struct update_output {
    //   sint32 returnCode;
    // };
    // // lookup (function)

    struct lookup_input {
        bit_1024 queryString;
        //   QNSName query;
    };

    struct lookup_output {
        sint32 returnCode;
        //Array<uint8, 128> returnString;
        QNSEntry returnEntry;
        // QNSEntry value;
        // sint32 returnCode;
    };

    // transferOwnership

    // struct transferOwnership_input {
    //   QNSEntry entry;
    // };

    // struct transferOwnership_output {
    //   sint32 returnCode;
    // };

    /**************************************/
    /************CORE FUNCTIONS************/
    /**************************************/

    /***** registerName *****/

    // Procedure to register a new name with a Qubic ID and URL
    PUBLIC_PROCEDURE_WITH_LOCALS(registerName)

        locals.logger._contractIndex = CONTRACT_INDEX;
        locals.logger._type = 0;
        locals.logger.originator = qpi.originator();
        locals.logger.id = input.entry.id;
        locals.logger.owner = input.entry.owner;
        LOG_INFO(locals.logger);
        // Check if the name is already registered
        if (state.lookupMap.getElementIndex(input.name) != QPI::NULL_INDEX)
        {
            // Name already registered
            output.returnCode = -1;
            return;
        }

        // Check if entries Owner matches the originator
        if(input.entry.owner != qpi.originator())
        {
            // TODO: Discuss if it shall be allowed that the originator is not the owner while registe a new domain
            output.returnCode = -1;
        }

        // Register the name
        state.lookupMap.set(input.name, input.entry);
        output.returnCode = 0;
    _


    // /***** update *****/

    // struct update_locals {
    //   QNSEntry value;
    // };

    // // Procedure to update the ID of a registered name
    // PUBLIC_PROCEDURE_WITH_LOCALS(update)
    //   // Check if the name is registered and the invocator is the owner
    //   // If entry is found value will have the value of the found entry (See
    //   // QPI::HashMap)
    //   if (state.nameToID.get(input.entry.name, locals.value) && locals.value.owner != qpi.invocator()) // TODO check if rather originator is needed?
    //   {
    //       output.returnCode = -1;  // Name not registered or not owned by invocator
    //       return;
    //   }

    //   state.nameToID.set(input.entry.name, input.entry);
    //   output.returnCode = 0; // Update successful
    // _

    /****** lookup *****/

    // Function to lookup the Qubic ID and URL by name
    PUBLIC_FUNCTION(lookup)
        //  Check if the name is registered
        if (state.lookupMap.get(input.queryString, output.returnEntry))
        {
            // Name was found
            output.returnCode = 0;
        }
        else
        {
            // Name not found
            // TODO: What to return if no entry is found
            output.returnCode = -1;
        }
    _

    /***** ownerTransfer *****/

    // struct transferOwnership_locals {
    //   QNSEntry value;
    // };

    // // Function to transfer ownership of a name
    // PUBLIC_PROCEDURE_WITH_LOCALS(transferOwnership)
    //   // Check if the name is registered and the invocator is the owner
    //   if (state.nameToID.get(input.entry.name, locals.value) && locals.value.owner != qpi.invocator()) {
    //       output.returnCode = -1; // Name not registered or not owned by invocator
    //       return;
    //   }

    //   // Transfer ownership
    //   // Actually it is a update of all fields
    //   state.nameToID.set(input.entry.name, locals.value);
    //   output.returnCode = 0; // Ownership transfer successful
    // _

    // Register the user functions and procedures
    REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_FUNCTION(lookup, 1);
        REGISTER_USER_PROCEDURE(registerName, 1);
        // REGISTER_USER_PROCEDURE(update, 2);
        // REGISTER_USER_PROCEDURE(transferOwnership, 4);
    _

};
