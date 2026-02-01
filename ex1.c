#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//TODO create functions that you can use to clean up the file
/* Struct for a person */
typedef struct Person {
	char *firstName;
	char *lastName;
	char *fingerprint;  // exactly 9 chars
	char *position;
	int arrivalOrder;
	struct Person *next;
} Person;

/* Safe strdup replacement */
char* my_strdup(const char *s) {
	if (!s) return NULL;
	char *d = malloc((strlen(s) + 1) * sizeof(char));
	if (d) memcpy(d, s, (strlen(s) + 1) * sizeof(char));
	return d;
}

/* Remove corruption characters */
void clean_string(char *s) {
	if (!s) return;
	char *dst = s;
	for (; *s; s++) {
		if (*s != '#' && *s != '?' && *s != '!' && *s != '@' &&
			*s != '&' && *s != '$' && *s != '\r' && *s != '\n') {
			*dst++ = *s;
		}
	}
	*dst = '\0';
}

/* Trim leading and trailing spaces in-place */
void trim_inplace(char *s) {
	if (!s) return;
	char *start = s;
	while (*start && isspace((unsigned char)*start)) start++;
	if (start != s) memmove(s, start, strlen(start) + 1);
	size_t len = strlen(s);
	while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
}

/* Position rank for sorting */
int getRank(const char *pos) {
	if (strcmp(pos, "Boss") == 0) return 1;
	if (strcmp(pos, "Right Hand") == 0) return 2;
	if (strcmp(pos, "Left Hand") == 0) return 3;
	if (strcmp(pos, "Support_Right") == 0) return 4;
	if (strcmp(pos, "Support_Left") == 0) return 5;
	return 6;
}

/* Fix position to standard names */
char* fix_position(const char *pos) {
	if (strstr(pos, "Boss")) return my_strdup("Boss");
	if (strstr(pos, "Right") && strstr(pos, "Hand")) return my_strdup("Right Hand");
	if (strstr(pos, "Left") && strstr(pos, "Hand")) return my_strdup("Left Hand");
	if (strstr(pos, "Support") && strstr(pos, "Right")) return my_strdup("Support_Right");
	if (strstr(pos, "Support") && strstr(pos, "Left")) return my_strdup("Support_Left");
	return my_strdup(pos);
}

/* Check for duplicate fingerprint */
int duplicate_fingerprint(Person *list, const char *fp) {
	for (Person *p = list; p; p = p->next)
		if (strcmp(p->fingerprint, fp) == 0) return 1;
	return 0;
}

/* Sort linked list by position rank and arrival order */
Person* sort_list(Person *head) {
	if (!head || !head->next) return head;
	Person *sorted = NULL;
	while (head) {
		Person *curr = head; head = head->next;
		if (!sorted || getRank(curr->position) < getRank(sorted->position) ||
			(getRank(curr->position) == getRank(sorted->position) &&
				curr->arrivalOrder < sorted->arrivalOrder)) {
			curr->next = sorted; sorted = curr;
		}
		else {
			Person *p = sorted;
			while (p->next && (getRank(p->next->position) < getRank(curr->position) ||
				(getRank(p->next->position) == getRank(curr->position) &&
					p->next->arrivalOrder < curr->arrivalOrder)))
				p = p->next;
			curr->next = p->next; p->next = curr;
		}
	}
	return sorted;
}

/* Free memory of linked list */
void free_list(Person *list) {
	while (list) {
		Person *tmp = list; list = list->next;
		free(tmp->firstName); free(tmp->lastName);
		free(tmp->fingerprint); free(tmp->position); free(tmp);
	}
}


int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: %s <input_corrupted.txt> <output_clean.txt>\n", argv[0]);
		return 0;
	}
	// TODO: implement
   FILE *fin = fopen(argv[1], "r");
    if (!fin) {
        printf("Error opening file: %s\n", argv[1]);
        return 0;
    }

    fseek(fin, 0, SEEK_END);
    long fsize = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    char *buffer = malloc(fsize + 1);
    if (!buffer) {
        printf("Error allocating memory\n");
        fclose(fin);
        return 0;
    }

    fread(buffer, 1, fsize, fin);
    buffer[fsize] = '\0';
    fclose(fin);

    clean_string(buffer);

    const char *labels[4] = { "First Name:", "Second Name:", "Fingerprint:", "Position:" };
    char *ptr = buffer;
    Person *list = NULL;
    int order = 0;

    while ((ptr = strstr(ptr, labels[0]))) {
        char *s1 = ptr + strlen(labels[0]);
        char *s2 = strstr(s1, labels[1]);
        char *s3 = s2 ? strstr(s2, labels[2]) : NULL;
        char *s4 = s3 ? strstr(s3, labels[3]) : NULL;
        if (!s2 || !s3 || !s4) break;

        char *next = strstr(s4 + strlen(labels[3]), labels[0]);

        size_t len_fn = s2 - s1;
        size_t len_sn = s3 - (s2 + strlen(labels[1]));
        size_t len_fp = s4 - (s3 + strlen(labels[2]));
        size_t len_ps = next ? (size_t)(next - (s4 + strlen(labels[3]))) : strlen(s4 + strlen(labels[3]));

        char *fn = malloc(len_fn + 1);
        char *sn = malloc(len_sn + 1);
        char *fp = malloc(len_fp + 1);
        char *ps_raw = malloc(len_ps + 1);

        if (!fn || !sn || !fp || !ps_raw) {
            printf("Error allocating memory\n");
            free(fn); free(sn); free(fp); free(ps_raw);
            free_list(list);
            free(buffer);
            return 0;
        }

        strncpy(fn, s1, len_fn); fn[len_fn] = '\0';
        strncpy(sn, s2 + strlen(labels[1]), len_sn); sn[len_sn] = '\0';
        strncpy(fp, s3 + strlen(labels[2]), len_fp); fp[len_fp] = '\0';
        strncpy(ps_raw, s4 + strlen(labels[3]), len_ps); ps_raw[len_ps] = '\0';

        trim_inplace(fn);
        trim_inplace(sn);
        trim_inplace(fp);
        trim_inplace(ps_raw);
        char *ps = fix_position(ps_raw);
        free(ps_raw);

        if (!duplicate_fingerprint(list, fp)) {
            Person *p = malloc(sizeof(Person));
            if (!p) {
                printf("Error allocating memory\n");
                free(fn); free(sn); free(fp); free(ps);
                free_list(list);
                free(buffer);
                return 0;
            }
            p->firstName = fn;
            p->lastName = sn;
            p->fingerprint = fp;
            p->position = ps;
            p->arrivalOrder = order++;
            p->next = list;
            list = p;
        } else {
            free(fn); free(sn); free(fp); free(ps);
        }

        ptr = next ? next : s4 + strlen(labels[3]);
    }

    free(buffer);

    list = sort_list(list);

    FILE *fout = fopen(argv[2], "w");
    if (!fout) {
        printf("Error opening file: %s\n", argv[2]);
        free_list(list);
        return 0;
    }

    for (Person *p = list; p; p = p->next)
        fprintf(fout, "First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: %s\n\n",
                p->firstName, p->lastName, p->fingerprint, p->position);

    fclose(fout);
    free_list(list);

    return 0;
}
