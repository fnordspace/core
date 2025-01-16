#include "contracts/qpi.h"
using namespace QPI;
//#include "../contract_core/qpi_hash_map_impl.h"
// Qubic Name Service Contract


// constexpr unsigned long long QNS_NAME_LENGTH = 256;

// // Datatype for the Name that is used in QNS e.g. myAwesomeName.qubic
// //using QNSName = uint8[QNS_NAME_LENGTH];
// using QNSOwner = QPI::id;
// using IPFSHash = m256i;


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


// Structure of each entry behind the lookup.
// struct QNSEntry {
//   // Name of the entry. Potentially needed to check for hash collisions
//   QNSName name;
//   // Id to look up
//   QPI::id id;
//   // Owner
//   QNSOwner owner;
//   // ipfs hash
//   IPFSHash ipfs;
//   // Expiration date
//   QPI::uint8 expiration;
// };

struct QNS2 {};

// Define state of the contract
struct QNS : public ContractBase {

  /**************************************/
  /************CONTRACT STATES***********/
  /**************************************/

  // Mapping from names to Qubic IDs
  // QPI::collection<QPI::id, 1024> nameToID;
  // QPI::HashMap<QNSName, QNSEntry, 1024> nameToID;
    QPI::Array<uint8, 128> exampleString;

  /**************************************/
  /********INPUT AND OUTPUT STRUCTS******/
  /**************************************/

  // Register

  // struct registerName_input {
  //   QNSEntry entry;
  // };

  // struct registerName_output {
  //   sint32 returnCode;
  // };

  // // Update

  // struct update_input {
  //   QNSEntry entry;
  // };

  // struct update_output {
  //   sint32 returnCode;
  // };
  // // lookup (function)

  struct lookup_input {
      uint32 query;
      Array<uint8, 128> queryString;
      //   QNSName query;
  };

  struct lookup_output {
      uint32 returnCode;
      Array<uint8, 128> returnString;
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
  // PUBLIC_PROCEDURE(registerName)

  //   // Check if the name is already registered
  //   if (state.nameToID.getElementIndex(input.entry.name) != QPI::NULL_INDEX)
  //   {
  //       output.returnCode = -1; // Name already registered
  //       return;
  //   }

  //   // Register the name
  //   state.nameToID.set(input.entry.name, input.entry);

  //   output.returnCode = 0; // Registration successful
  // _


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
    // if (!state.exampleMap.get(input.query, output.returnCode))
    if(true)
    {
        output.returnCode = input.query; // Name not found return;
        output.returnString.setAll(0);
        output.returnString.setMem(input.queryString);
        return;
    }
  _


  // struct INITIALIZE_LOCALS{
  //     char t1[265];
  //     char t2[265];
  // };

  // INITIALIZE_WITH_LOCALS
  //   locals.t1 = {'h', 'e', 'l', 'l', 'o','\0'};
  //   locals.t2 = {'h', 'o', 'l', 'a','\0'};
  //   state.exampleMap.set(locals.t1, 42);
  //   state.exampleMap.set(locals.t2, 19);
  // _

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
    // REGISTER_USER_PROCEDURE(registerName, 1);
    // REGISTER_USER_PROCEDURE(update, 2);
    // REGISTER_USER_PROCEDURE(transferOwnership, 4);
  _

};
