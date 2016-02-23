// String Encryption PoC by Tomislav Zubcic
// chaplja.blogspot.com | chaplja@gmail.com

#pragma once

// standard C headers
#include <cstdint>
#include <cstdlib>

// string prefix	[          header         ] [             key                ] [     length     ] [ is_encrypted = 0 ]
#define SE_PFX		"\x00\xFF" "K" "\xFF\x00" "\x00\x00\x22\x22\x00\x00\x00\x00" "\x00\x00\x00\x00" "\x00"

// encrypt a string in code
#define enc(x)		se_decrypt(SE_PFX x)

#pragma pack(push, 1)

typedef struct {
	uint8_t		header[5];
	uint8_t		key[2];
	uint32_t	length;
	uint8_t		is_encrypted;
} se_string_t;

#pragma pack(pop)

__forceinline const char *se_decrypt(const char * volatile str)
{
	se_string_t		*ss;
	const char		*real_str;

	// string is prefixed with our structure
	ss = (se_string_t *)str;
	real_str = str + sizeof(se_string_t);

	// this check is not really needed, but allows the strings to work properly during development
	// so you don't have to pass the binary every time to the encryption tool
	if (ss->is_encrypted) {
		char	*new_str;

		// allocate a new string
		new_str = (char *)malloc(ss->length + 1);

		// decrypt it
		for (unsigned i = 0; i < ss->length; i++) {
			new_str[i] = real_str[i] ^ ss->key[i % sizeof(ss->key)];
		}

		// append nul char
		new_str[ss->length] = 0;

		// return decrypted string
		real_str = new_str;
	}

	return real_str;
}
