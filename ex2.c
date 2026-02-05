#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "org_tree.h"
#define FP_LEN 9


static void print_success(int mask, char *op, char* fingerprint, char* First_Name, char* Second_Name)
{
	printf("Successful Decrypt! The Mask used was mask_%d of type (%s) and The fingerprint was %.*s belonging to %s %s\n",
		mask, op, FP_LEN, fingerprint, First_Name, Second_Name);
}

static void print_unsuccess()
{
	printf("Unsuccesful decrypt, Looks like he got away\n");
}

int main(int argc, char **argv) {
	if (argc != 4) {
		printf("Usage: %s <clean_file.txt> <cipher_bits.txt> <mask_start_s>\n", argv[0]);
		return 0;
	}
	// TODO: read the input files
	unsigned char cipher[FP_LEN];
	FILE *f_cipher = fopen(argv[2], "r");

	if (!f_cipher)
	{
		printf("Error opening file: %s\n", argv[2]);
		return 0;
	}

	char line[128];
	for (int i = 0; i < FP_LEN; i++) {
		if (!fgets(line, sizeof(line), f_cipher)) break;

		// Clean newline characters
		line[strcspn(line, "\r\n")] = '\0';

		// Convert binary string
		unsigned char val = 0;
		for (int b = 0; b < 8; b++) {
			val <<= 1;
			if (line[b] == '1') val |= 1;
		}
		cipher[i] = val;
	}
	fclose(f_cipher);

	// TODO: build the organization
	Org org = build_org_from_clean_file(argv[1]);
	// TODO: attempt to decrypt the file
	int start_mask;
	FILE *mask_fp = fopen(argv[3], "r");

	if (mask_fp) {
		if (fscanf(mask_fp, "%d", &start_mask) != 1) start_mask = atoi(argv[3]);
		fclose(mask_fp);
	}
	else {
		start_mask = atoi(argv[3]);
	}

	int found = 0;
	int mask, op, h;

	// Loop through masks s to s+10
	for (mask = start_mask; mask <= start_mask + 10 && !found; mask++) {

		// Try XOR (op=1) and AND (op=0)
		for (op = 0; op < 2 && !found; op++) {
			char *op_name = (op == 1) ? "XOR" : "AND";

			// Nodes to check: Boss, Left Hand, Right Hand
			Node *heads[3] = { org.boss, org.left_hand, org.right_hand };

			for (h = 0; h < 3 && !found; h++) {
				Node *curr_head = heads[h];
				if (!curr_head) continue;

				// Traverse head and its supports list
				Node *curr_node = curr_head;
				int is_head = 1;

				while (curr_node && !found) {
					int match = 1;
					for (int i = 0; i < FP_LEN; i++) {
						unsigned char plain_byte = (unsigned char)curr_node->fingerprint[i];
						unsigned char result = (op == 1) ? (plain_byte ^ (unsigned char)mask)
							: (plain_byte & (unsigned char)mask);

						if (result != cipher[i]) {
							match = 0;
							break;
						}
					}

					if (match) {
						print_success(mask, op_name, curr_node->fingerprint, curr_node->first, curr_node->second);
						found = 1;
						break;
					}

					// Linked list logic: move from head to supports list
					if (is_head) {
						curr_node = curr_head->supports_head;
						is_head = 0;
					}
					else {
						curr_node = curr_node->next;
					}
				}
			}
		}
	}

	if (!found) {
		print_unsuccess();
	}
	// TODO: free any memory you may have allocated
	free_org(&org);
	return 0;
}
