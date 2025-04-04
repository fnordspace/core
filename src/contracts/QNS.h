#include "qpi.h"
using namespace QPI;
// Qubic Name Service Contract

// TODO:
// - SubDomains
// - Fees
// - domainname normalization and sanitazing (e.g., toLower, check for \0 and other special chars, emojis)



using IPFSHash = QPI::BitArray<256>; // 32 Byte IPF CID

// Basic struct of a Entry in the QNS Registry
struct QNSEntry {
  // Name of the entry. Potentially needed to check for hash collisions
  //QNSName name;
  // Id to look up
  QPI::id id;
  // Owner
  QPI::id  owner;
  // ipfs hash
  IPFSHash ipfsCID;
  // Expiration date
  QPI::uint16 expiration;
};


struct QNSStats{
    uint64 numberOfEntries;
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
    uint64 numberOfEntries;

    struct QNSLogger
    {
        uint32 _contractIndex;
        uint32 _type;
        // 0: register
        // 1: update
        // 2: updateExpiration
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
        QPI::uint16 numberOfEpochs;
    };

    // Update
    struct update_input {
      QPI::bit_1024 name;
      QNSEntry entry;
    };

    struct update_output {
      sint32 returnCode;
    };

    struct update_locals {
        QNSEntry entry;
        QPI::uint16 expiration;
        QNSLogger logger;
    };

    // UpdateExpiration
    struct updateExpiration_input {
      QPI::bit_1024 name;
        QPI::uint16 expiration;
    };

    struct updateExpiration_output {
      sint32 returnCode;
    };

    struct updateExpiration_locals {
        QNSEntry entry;
        QNSLogger logger;
    };

    // Lookup
    struct lookup_input {
        bit_1024 name;
    } ;

    struct lookup_output {
        sint32 returnCode;
        //Array<uint8, 128> returnString;
        QNSEntry returnEntry;
        // QNSEntry value;
        // sint32 returnCode;
    };

    struct getStats_input {
        // Dummy input struct
    };
    struct getStats_output {
        sint32 returnCode;
        //Array<uint8, 128> returnString;
        QNSStats stats;
        // QNSEntry value;
        // sint32 returnCode;
    };


    /**************************************/
    /************CORE FUNCTIONS************/
    /**************************************/

    /***** registerName *****/

    // Register a new domain
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
            // TODO: Discuss if it shall be allowed that the originator is not the owner while register a new domain
            output.returnCode = -1;
            //return; // At the moment it is allowed
        }

        // Check if expiration is a valid epoch.
        if(input.entry.expiration <= qpi.epoch()){
            output.returnCode = -1;
            return;
        }

        locals.numberOfEpochs = input.entry.expiration - qpi.epoch();
        // calculate fee

        // Register the name
        state.lookupMap.set(input.name, input.entry);
        state.numberOfEntries++;
        output.returnCode = 0;
    _


    // /***** update *****/

    // Update the entry of a registered domain
    // Expiration is not allowed to update in this procedure
    PUBLIC_PROCEDURE_WITH_LOCALS(update)
        // Check if name is registered
        locals.logger._contractIndex = CONTRACT_INDEX;
        locals.logger._type = 1;
        locals.logger.originator = qpi.originator();
        locals.logger.id = input.entry.id;
        locals.logger.owner = input.entry.owner;
        LOG_INFO(locals.logger);

        if (!state.lookupMap.get(input.name, locals.entry))
        {
            // Entry not found
            output.returnCode = -1;
            return;
        }
        // Check if originator is the owner of the domain
        if(locals.entry.owner != qpi.originator())
        {
            // originator is not the owner - no update possible
            output.returnCode = -1;
            return;
        }
        // Save expiration to value in the entry;
        input.entry.expiration = locals.entry.expiration;

        state.lookupMap.set(input.name, input.entry);

        output.returnCode = 0;
    _

    // /***** updateExpiration *****/

    // Update the entry of a registered domain
    // Expiration is not allowed to update in this procedure
    PUBLIC_PROCEDURE_WITH_LOCALS(updateExpiration)
        // Check if name is registered
        locals.logger._contractIndex = CONTRACT_INDEX;
        locals.logger._type = 2;
        locals.logger.originator = qpi.originator();
        locals.logger.id = m256i::zero();
        locals.logger.owner = m256i::zero();
        LOG_INFO(locals.logger);

        if (!state.lookupMap.get(input.name, locals.entry))
        {
            // Entry not found
            output.returnCode = -1;
            return;
        }
        // Check if originator is the owner of the domain
        if(locals.entry.owner != qpi.originator())
        {
            // originator is not the owner - no update possible
            output.returnCode = -1;
            return;
        }
        // Check if expiration is a valid epoch.
        if(input.expiration <= qpi.epoch()){
            output.returnCode = -1;
            return;
        }

        // Save expiration to value in the entry;
        locals.entry.expiration = input.expiration;

        state.lookupMap.set(input.name, locals.entry);

        output.returnCode = 0;
    _



    /****** lookup *****/
    // Lookup the entry of a domain
    PUBLIC_FUNCTION(lookup)
        //  Check if the name is registered
        if (state.lookupMap.get(input.name, output.returnEntry))
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

    /****** getStats *****/
    // get stats aoubt the registry
    PUBLIC_FUNCTION(getStats)
        output.stats.numberOfEntries = state.numberOfEntries;
        output.returnCode = 0;
    _

    INITIALIZE
        state.numberOfEntries = 0;
    _


    // Register the user functions and procedures
    REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_FUNCTION(lookup, 1);
        REGISTER_USER_FUNCTION(getStats, 2);
        REGISTER_USER_PROCEDURE(registerName, 1);
        REGISTER_USER_PROCEDURE(update, 2);
        REGISTER_USER_PROCEDURE(updateExpiration, 3);
        // REGISTER_USER_PROCEDURE(transferOwnership, 4);
    _

};
