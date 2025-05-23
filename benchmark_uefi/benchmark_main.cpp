#include "src/platform/m256.h"
#include "src/platform/time_stamp_counter.h"
#include "lib/platform_efi/uefi.h"
#include "lib/platform_common/compiler_warnings.h"

// Global SystemTable pointer
EFI_SYSTEM_TABLE *gSystemTable = nullptr;

// Helper function to convert unsigned long long to CHAR16 hex string
void ull_to_hex_str(unsigned long long n, CHAR16* out_str) {
    if (out_str == nullptr) return;

    const CHAR16 hex_chars[] = L"0123456789ABCDEF";
    // Max 16 hex digits for 64-bit number + null terminator
    CHAR16 buffer[17]; 
    int i = 15;
    buffer[16] = L'\0'; 

    if (n == 0) {
        out_str[0] = L'0';
        out_str[1] = L'\0';
        return;
    }

    while (n > 0 && i >= 0) {
        buffer[i--] = hex_chars[n % 16];
        n /= 16;
    }
    
    // Copy to output string, skipping leading zeros if any
    int j = 0;
    while(buffer[i+1+j] == L'0' && buffer[i+2+j] != L'\0') { // keep last zero if number is 0
        i++;
    }
    
    int k = 0;
    if (i < 0) i = 0; // handle case where n was 0 initially
    while(buffer[i+1+k] != L'\0') {
        out_str[k] = buffer[i+1+k];
        k++;
    }
    out_str[k] = L'\0';

    if (out_str[0] == L'\0' && n == 0) { // if original n was 0
         out_str[0] = L'0';
         out_str[1] = L'\0';
    } else if (out_str[0] == L'\0') { // If number was non-zero but resulted in empty string (e.g. due to buffer logic)
        // This indicates an issue with the loop or buffer indexing,
        // for safety, put "Error" or a known pattern.
        // For now, let's assume the logic above handles it, but this is a safeguard.
        const CHAR16 err_str[] = L"CONV_ERR";
        for(int l=0; err_str[l] != L'\0'; ++l) out_str[l] = err_str[l];
        out_str[sizeof(err_str)/sizeof(CHAR16) -1] = L'\0';
    }
}

// Helper to print a string followed by a newline
void print_line(const CHAR16* str) {
    if (gSystemTable && gSystemTable->ConOut) {
        gSystemTable->ConOut->OutputString(gSystemTable->ConOut, (CHAR16*)str);
        gSystemTable->ConOut->OutputString(gSystemTable->ConOut, L"\r\n");
    }
}

// Helper to print a string followed by a hex value and newline
void print_value_hex(const CHAR16* prefix, unsigned long long value) {
    if (gSystemTable && gSystemTable->ConOut) {
        CHAR16 buffer[256];
        CHAR16 value_str[20]; // For hex representation of u64

        ull_to_hex_str(value, value_str);

        // Simple concatenation
        int i = 0;
        while (prefix[i] != L'\0' && i < 200) { // 200 to leave space for value and terminator
            buffer[i] = prefix[i];
            i++;
        }
        buffer[i++] = L' '; // Add a space
        
        int j = 0;
        while (value_str[j] != L'\0' && i < 254) { // 254 to leave space for terminator
            buffer[i] = value_str[j];
            i++; j++;
        }
        buffer[i] = L'\0';

        print_line(buffer);
    }
}


EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    // Suppress unused parameter warnings
    (void)ImageHandle;

    // Store SystemTable globally
    gSystemTable = SystemTable;

    // Basic check for SystemTable and ConOut
    if (!gSystemTable || !gSystemTable->ConOut) {
        return EFI_UNSUPPORTED;
    }

    // Clear the screen
    gSystemTable->ConOut->ClearScreen(gSystemTable->ConOut);
    print_line(L"UEFI m256i Benchmark Application");
    print_line(L"=================================");

    // Initialize time-stamp counter
    initTimeStampCounter();
    print_line(L"Time-stamp counter initialized.");

    CHAR16 message[256];
    const unsigned long long iterations = 100000; // 100k iterations

    print_line(L"Starting m256i benchmark...");
    print_value_hex(L"Iterations:", iterations);


    // --- Benchmark Variables ---
    m256i a(1, 2, 3, 4);
    m256i b = a;
    m256i c;
    bool equal_res = false;
    
    unsigned long long start_tsc, end_tsc;
    unsigned long long total_cycles = 0;

    // --- Actual Benchmark Loop ---
    start_tsc = __rdtsc();

    for (unsigned long long i = 0; i < iterations; ++i) {
        // Construction (already done for 'a', re-init for timing consistency if desired, or time a block)
        // For this loop, we focus on operations with existing objects.
        m256i temp_a( (uint32_t)i, (uint32_t)(i+1), (uint32_t)(i+2), (uint32_t)(i+3) ); // Construction
        b = temp_a;                                 // Assignment
        equal_res = (temp_a == b);                  // Comparison
        // The boolean result isn't directly used here to prevent optimizing out,
        // but in a real scenario, you might sum results or similar.
        // For now, we rely on the operations themselves not being optimized away.
        if (!equal_res) { 
            // This should ideally not happen if assignment works.
            // Could print an error or increment a counter if it does.
        }
        a.setRandomValue();                         // setRandomValue
        c = m256i::zero();                          // zero
    }

    end_tsc = __rdtsc();
    total_cycles = end_tsc - start_tsc;

    // --- Results ---
    print_line(L"Benchmark finished.");
    print_value_hex(L"Total cycles:", total_cycles);

    if (iterations > 0) {
        unsigned long long avg_cycles_per_iteration = total_cycles / iterations;
        print_value_hex(L"Average cycles per iteration:", avg_cycles_per_iteration);
    } else {
        print_line(L"No iterations performed.");
    }
    
    // Example of printing one of the m256i values (first u32 element)
    // This would require m256i to have a getter or a way to access its elements.
    // For now, we'll skip printing m256i contents to keep it simple.
    // If m256i has a method like `get_element(0)` returning uint32_t:
    // print_value_hex(L"Value of a.elements[0] (example):", a.get_element(0)); 

    print_line(L"=================================");
    print_line(L"Benchmark complete. System will halt in a moment or press ESC to exit.");

    // Wait for a key press (optional, good for seeing output)
    // EFI_INPUT_KEY Key;
    // SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
    // while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key) == EFI_NOT_READY);

    return EFI_SUCCESS;
}
