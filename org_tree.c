#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "org_tree.h"


Org build_org_from_clean_file(const char *path) {
	// TODO: implement
	
		Org org = { 0 };

		FILE *fin = fopen(path, "r");
		if (!fin) {
			printf("Error opening file: %s\n", path);
			return org;
		}

		char line[MAX_FIELD];
		char first[MAX_FIELD], second[MAX_FIELD], fingerprint[MAX_FIELD];
		char position[MAX_POS];

		Node *last_support_left = NULL;
		Node *last_support_right = NULL;

		while (fgets(line, sizeof(line), fin)) {

			if (sscanf(line, "First Name: %[^\n]", first) != 1)
				continue;

			if (!fgets(line, sizeof(line), fin) ||
				sscanf(line, "Second Name: %[^\n]", second) != 1)
				break;

			if (!fgets(line, sizeof(line), fin) ||
				sscanf(line, "Fingerprint: %[^\n]", fingerprint) != 1)
				break;

			if (!fgets(line, sizeof(line), fin) ||
				sscanf(line, "Position: %[^\n]", position) != 1)
				break;

			fgets(line, sizeof(line), fin); /* blank line */

			Node *node = malloc(sizeof(Node));
			if (!node) {
				printf("Error allocating memory\n");
				free_org(&org);
				fclose(fin);
				return org;
			}

			strncpy(node->first, first, MAX_FIELD - 1);
			node->first[MAX_FIELD - 1] = '\0';

			strncpy(node->second, second, MAX_FIELD - 1);
			node->second[MAX_FIELD - 1] = '\0';

			strncpy(node->fingerprint, fingerprint, MAX_FIELD - 1);
			node->fingerprint[MAX_FIELD - 1] = '\0';

			strncpy(node->position, position, MAX_POS - 1);
			node->position[MAX_POS - 1] = '\0';

			node->left = node->right = node->supports_head = node->next = NULL;

			if (strcmp(position, "Boss") == 0) {
				if (!org.boss)
					org.boss = node;
				else
					free(node);
			}
			else if (strcmp(position, "Left Hand") == 0) {
				if (!org.left_hand) {
					org.left_hand = node;
					if (org.boss) org.boss->left = node;
					last_support_left = NULL;
				}
				else {
					free(node);
				}
			}
			else if (strcmp(position, "Right Hand") == 0) {
				if (!org.right_hand) {
					org.right_hand = node;
					if (org.boss) org.boss->right = node;
					last_support_right = NULL;
				}
				else {
					free(node);
				}
			}
			else if (strcmp(position, "Support_Left") == 0) {
				if (!org.left_hand) {
					free(node);
					continue;
				}
				if (!org.left_hand->supports_head)
					org.left_hand->supports_head = node;
				else
					last_support_left->next = node;

				last_support_left = node;
			}
			else if (strcmp(position, "Support_Right") == 0) {
				if (!org.right_hand) {
					free(node);
					continue;
				}
				if (!org.right_hand->supports_head)
					org.right_hand->supports_head = node;
				else
					last_support_right->next = node;

				last_support_right = node;
			}
			else {
				free(node);
			}
		}

		fclose(fin);
		return org;
	
}

void print_tree_order(const Org *org) 
{
	if (!org) return;

	if (org->boss) {
		printf("First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: %s\n\n",
			org->boss->first, org->boss->second,
			org->boss->fingerprint, org->boss->position);
	}

	if (org->left_hand) {
		printf("First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: %s\n\n",
			org->left_hand->first, org->left_hand->second,
			org->left_hand->fingerprint, org->left_hand->position);

		Node *curr = org->left_hand->supports_head;
		while (curr) {
			printf("First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: %s\n\n",
				curr->first, curr->second,
				curr->fingerprint, curr->position);
			curr = curr->next;
		}
	}

	if (org->right_hand) {
		printf("First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: %s\n\n",
			org->right_hand->first, org->right_hand->second,
			org->right_hand->fingerprint, org->right_hand->position);

		Node *curr = org->right_hand->supports_head;
		while (curr) {
			printf("First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: %s\n\n",
				curr->first, curr->second,
				curr->fingerprint, curr->position);
			curr = curr->next;
		}
	}
}
void free_org(Org *org) {
    // TODO: implement

		if (!org) return;

		// Free left-hand support list
		if (org->left_hand) {
			Node *curr = org->left_hand->supports_head;
			while (curr) {
				Node *tmp = curr;
				curr = curr->next;
				free(tmp);
			}
			free(org->left_hand);
		}

		// Free right-hand support list
		if (org->right_hand) {
			Node *curr = org->right_hand->supports_head;
			while (curr) {
				Node *tmp = curr;
				curr = curr->next;
				free(tmp);
			}
			free(org->right_hand);
		}

		// Free boss
		if (org->boss) free(org->boss);

		// Reset pointers to NULL (optional but safer)
		org->boss = NULL;
		org->left_hand = NULL;
		org->right_hand = NULL;
}

