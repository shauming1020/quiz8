#include <stdio.h>
#include <time.h>
#include "list.h"

int file_exists(const char *fname) {
    FILE *file;
    if ((file = fopen(fname, "r"))) {
        printf("test file is exist ... \n");
        fclose(file);
        return 1;
    }
    return 0;
}

double tvgetf()
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;
    return sec;
}

static void print_key(list_node_t *n) {
    printf("key, val = %s, %s \n", n->key, n->val);
}

static void rand_key(char *str, size_t size) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    while(size-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *str++ = charset[index];
    }
    *str = '\0';
}

void generate_sparse_test_file(int sampletimes, int MAX_LEN, char *IN_FILE) {
    FILE *fp;
    fp = fopen(IN_FILE, "w");
    int l;
    char *key = malloc(MAX_LEN + 1);

    while(sampletimes--) {
        l = (double) rand() / RAND_MAX * MAX_LEN + 1;
        rand_key(key, l);
        fprintf(fp, "%s\n", key);
    }
    printf("test file %s is generated ... \n", IN_FILE);
}

void test_insert(skiplist *d, int mode, FILE *rep_file, int MAX_LEN, char *IN_FILE) {
    FILE *fp;
    fp = fopen(IN_FILE, "r");

    double t1, t2;
    char buf[MAX_LEN + 2];

    if (mode == 0) {
        printf("Insert with hash function... \n");
        t1 = tvgetf();
        while(fgets(buf, MAX_LEN + 2, fp)) {
            char *key = malloc(MAX_LEN + 1), *val = malloc(MAX_LEN + 1);

            for (int i = 0; i < MAX_LEN + 1; i++) {
                key[i] = (buf[i] == '\n') ? '\0' : buf[i];
                val[i] = (buf[i] == '\n') ? '\0' : buf[i];
            }

            LIST_INSERT_HASHFUNC(d, key, MAX_LEN, val);
        }
        t2 = tvgetf();

    } else if (mode == 1) {
        printf("Insert with rand level... \n");
        t1 = tvgetf();
        while(fgets(buf, MAX_LEN + 2, fp)) {
            char *key = malloc(MAX_LEN + 1), *val = malloc(MAX_LEN + 1);

            for (int i = 0; i < MAX_LEN + 1; i++) {
                key[i] = (buf[i] == '\n') ? '\0' : buf[i];
                val[i] = (buf[i] == '\n') ? '\0' : buf[i];
            }

            LIST_INSERT_RANDLEVEL(d, key, MAX_LEN, val);
        }
        t2 = tvgetf();
    } else if (mode == 2) {
        printf("Insert with xorshift ... \n");
        t1 = tvgetf();
        while(fgets(buf, MAX_LEN + 2, fp)) {
            char *key = malloc(MAX_LEN + 1), *val = malloc(MAX_LEN + 1);

            for (int i = 0; i < MAX_LEN + 1; i++) {
                key[i] = (buf[i] == '\n') ? '\0' : buf[i];
                val[i] = (buf[i] == '\n') ? '\0' : buf[i];
            }
            LIST_INSERT_XORSHIFT(d, key, MAX_LEN, val);
        }
        t2 = tvgetf();
    }

    fprintf(rep_file, "Insert total cost %.6f msec \n", (t2 - t1) * 1000);

}

void test_find(skiplist *d, FILE *rep_file, int MAX_LEN, char *IN_FILE) {
    FILE *fp;
    fp = fopen(IN_FILE, "r");

    char buf[MAX_LEN + 2];
    double t1, t2;

    t1 = tvgetf();
    while (fgets(buf, MAX_LEN + 2, fp)) {
        custom_t v;
        char *key = malloc(MAX_LEN + 1);

        for (int i = 0; i < MAX_LEN + 1; i++)
            key[i] = (buf[i] == '\n') ? '\0' : buf[i];

        LIST_GET(d, key, MAX_LEN, v);

    }
    t2 = tvgetf();
    fprintf(rep_file, "Find all total cost %f msec\n", (t2 - t1) * 1000);

}

void test_delete(skiplist *d, FILE *rep_file, int MAX_LEN, char *IN_FILE) {
    FILE *fp;
    fp = fopen(IN_FILE, "r");

    double t1, t2;
    char buf[MAX_LEN + 2];

    t1 = tvgetf();
    while (fgets(buf, MAX_LEN + 2, fp)) {
        char *key = malloc(MAX_LEN + 1);

        for (int i = 0; i < MAX_LEN + 1; i++)
            key[i] = (buf[i] == '\n') ? '\0' : buf[i];

        LIST_DELETE(d, key, MAX_LEN);

    }
    t2 = tvgetf();

    fprintf(rep_file, "Delete all total cost %f msec\n", (t2 - t1) * 1000);
}

int main(int argc, const char* argv[])
{
    /* */
    if (argc != 6) {
    printf("Usage: num elements, MAX_LEN, insert mode : 0 : HASHFUNC, 1 : RANDLEVEL, 2 : XORSHIFT, test file name, report file name \n");
    return 1;
    }

    int num_elements = atoi(argv[1]);
    int MAX_LEN = atoi(argv[2]);
    int insert_mode = atoi(argv[3]);
    char *IN_FILE = (char *) argv[4];
    const char *REPORT_FILE = argv[5];

    /* test file and report file*/
    if (!file_exists(IN_FILE)) {
        /* sample and output the test data*/
        generate_sparse_test_file(num_elements, MAX_LEN, IN_FILE);
    }

    FILE *rep_file;
    rep_file = fopen(REPORT_FILE, "w");

    /* initialize the list */
    skiplist *d;
    LIST_INIT(d);

    /* build and test skip list d */
    test_insert(d, insert_mode, rep_file, MAX_LEN, IN_FILE); // 0 : HASHFUNC, 1 : RANDLEVEL, 2 : XORSHIFT
    test_find(d, rep_file, MAX_LEN, IN_FILE);
    test_delete(d, rep_file, MAX_LEN, IN_FILE);

    /* iterate through the list and print */
    /*
    double t1, t2;
    t1 = tvgetf();
    LIST_ITERATE(d, print_key);
    t2 = tvgetf();
    fprintf(rep_file, "Iterate all node and print total cost %f msec\n", (t2 - t1) * 1000);

    test_delete(d, rep_file, MAX_LEN, IN_FILE);

    printf("After delete, iterator \n");
    LIST_ITERATE(d, print_key);
    */

    printf("done \n");
    return 0;
}
