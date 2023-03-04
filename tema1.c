#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
pthread_mutex_t mutex;

void clean_line(char *line)
{
    int len = 0;
    len = strlen(line);
    if (line[len - 1] == '\n')
    {
        line[len - 1] = '\0';
        if (line[len - 2] == '\r')
            line[len - 2] = '\0';
    }
}

typedef struct
{
    char *path;
    int processed;
} *Tfile, objfile;

typedef struct
{
    int nr;
    char *path;
    int nr_reducers;
    TLista *array_of_lists;
    int nr_of_files;
    Tfile *input_files;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;

} *Mapper, objMapper;

typedef struct
{
    int nr; // id de identificare+coloana dupa care o sa fac reduce
    int nr_mappers;
    TLista **matrix_results; // matricea de rezultate de la mappers
    TLista result;
    int counter;
    pthread_barrier_t *barrier;
} *Reducer, objReducer;

int check_perf_pow(int nr, int exponent_candidate)
{
    unsigned long long low = 1;
    unsigned long long high = nr;

    while (low <= high)
    {
        unsigned long long middle = low + (high - low) / 2;
        unsigned long long built = middle;

        for (int i = 1; i < exponent_candidate; i++)
        {
            built *= middle;

            if (built > nr)
                break;
        }

        if (built > nr)
            high = middle - 1;
        else
            low = middle + 1;

        // the number is a perfect number
        if (built == nr)
            return 1;
    }

    // the number is not a perfect power
    return -1;
}

void *reduce_colon(void *args)
{
    Reducer startargs = (Reducer)args;
    int col = startargs->nr;
    pthread_barrier_wait(startargs->barrier);

    for (int i = 0; i < startargs->nr_mappers; i++)
    {
        appendList(&startargs->result, startargs->matrix_results[i][col]);
        bubbleSort(startargs->result);
        removeDuplicates(startargs->result);
        startargs->counter = getCount(startargs->result);
    }

    // TODO: create output file ->write answer
    char *name = calloc(20, sizeof(char));
    char file_nr[20];
    FILE *fp;
    sprintf(file_nr, "%d", col + 2);

    strcpy(name, "out");
    strcat(name, file_nr);
    strcat(name, ".txt");

    fp = fopen(name, "wr+");
    fprintf(fp, "%d", startargs->counter);
    fclose(fp);
    free(name);
}

// process one input file
void *process_input_file(void *args)
{
    Mapper startargs = (Mapper)args;

    char *path = startargs->path;
    int nr_reducers = startargs->nr_reducers;

    FILE *f = NULL;
    char *input_file = calloc(20, sizeof(char));
    size_t len = 0;
    int total_nr;
    int nr;

    TLista *array_of_lists = startargs->array_of_lists;

    f = fopen(path, "rt");
    if (f == NULL)
        return NULL;

    fscanf(f, "%d\n", &total_nr);

    for (int i = 0; i < total_nr; i++)
    {
        fscanf(f, "%d\n", &nr);

        // TODO: if nr==1, add to all lists
        if (nr == 1)
        {
            for (int i = 0; i < nr_reducers; i++)
                addToList(&array_of_lists[i], nr);
        }

        // loop through all exponents that nr can be a perfect power of
        if (nr != 1 && nr != 0)
            for (int exponent_candidate = 2; exponent_candidate <= nr_reducers + 1; exponent_candidate++)
            {
                int is_perf_pow = check_perf_pow(nr, exponent_candidate);

                // TODO: add nr to list of index: exponent_candidate
                if (is_perf_pow == 1)
                    addToList(&array_of_lists[exponent_candidate - 2], nr);
            }
    }

    fclose(f);
    free(input_file);
}



void *process(void *arg)
{
    Mapper mapper = (Mapper)arg;

    int counter = 0;
    while (counter < mapper->nr_of_files)
    {
        if (mapper->input_files[counter]->processed != 1) // found unprocessed file
        {
            pthread_mutex_lock(&mutex);                  // block memory block
            mapper->input_files[counter]->processed = 1; // mark as processed

            mapper->path = calloc(10, sizeof(char));
            strcpy(mapper->path, mapper->input_files[counter]->path); // save the file ti be processed name

            process_input_file(mapper); // send to process

            pthread_mutex_unlock(&mutex); // unlock memory block
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            counter++; // go to next file
        }
    }
    pthread_barrier_wait(mapper->barrier);
    pthread_exit(NULL);
}

int process_all_input_files(char *path, int nr_reducers, int nr_mappers)
{
    FILE *f = NULL;
    char *input_file;
    size_t len = 0;
    int r;

    f = fopen(path, "rt");
    if (f == NULL)
        return -1;

    int nr_of_files;
    fscanf(f, "%d\n", &nr_of_files);

    pthread_t threads[nr_mappers];
    pthread_t threads_r[nr_reducers];

    pthread_barrier_t bar;
    pthread_barrier_init(&bar, NULL, nr_mappers + nr_reducers);

    // initialize array of Tfile structs
    Tfile *input_files = calloc(nr_of_files, sizeof(Tfile));

    for (int i = 0; i < nr_of_files; i++)
    {
        input_files[i] = calloc(1, sizeof(objfile));

        getline(&input_file, &len, f);
        clean_line(input_file);

        input_files[i]->path = calloc(30, sizeof(char));
        strcpy(input_files[i]->path, input_file);
    }

    // initialize array of mappers + create threads for mappers
    Mapper *mappers = calloc(nr_mappers, sizeof(Mapper));

    for (int i = 0; i < nr_mappers; i++)
    {
        mappers[i] = calloc(1, sizeof(objMapper));
        mappers[i]->nr = i; // marchez id-ul mapperului

        TLista *array_of_lists = calloc(nr_reducers, sizeof(TLista));
        for (int i = 0; i < nr_reducers; i++)
            array_of_lists[i] = NULL;

        mappers[i]->array_of_lists = array_of_lists;
        mappers[i]->nr_reducers = nr_reducers;
        mappers[i]->input_files = input_files;
        mappers[i]->nr_of_files = nr_of_files;
        mappers[i]->barrier = &bar;

        r = pthread_create(&threads[i], NULL, process, mappers[i]);
    }

    // matrix with all results from mappers
    TLista **matrix_results = calloc(nr_mappers, sizeof(TLista *));
    for (int i = 0; i < nr_mappers; i++)
    {
        matrix_results[i] = calloc(nr_reducers, sizeof(TLista));
        matrix_results[i] = mappers[i]->array_of_lists;
    }

    // initialize all reducers + create reducers threads
    Reducer *reducers = calloc(nr_reducers, sizeof(Reducer));
    for (int i = 0; i < nr_reducers; i++)
    {
        reducers[i] = calloc(1, sizeof(objReducer));
        reducers[i]->nr = i;
        reducers[i]->matrix_results = matrix_results;
        reducers[i]->result = NULL;
        reducers[i]->nr_mappers = nr_mappers;
        reducers[i]->counter = 0;
        reducers[i]->barrier = &bar;

        r = pthread_create(&threads_r[i], NULL, reduce_colon, reducers[i]);
    }

    // start all threads
    for (int i = 0; i < nr_mappers; i++)
        r = pthread_join(threads[i], NULL);
    for (int i = 0; i < nr_reducers; i++)
        r = pthread_join(threads_r[i], NULL);

    pthread_barrier_destroy(&bar);

    fclose(f);

    free(mappers);
    free(input_files);
    free(input_file);
}

int main(int argc, char **argv)
{
    int nr_mappers = atoi(argv[1]);
    int nr_reducers = atoi(argv[2]);

    char *file_name = (char *)calloc(20, sizeof(char));
    strcpy(file_name, argv[3]);
    clean_line(file_name);

    process_all_input_files(file_name, nr_reducers, nr_mappers);

    free(file_name);
}