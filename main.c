// ATTENTION! TOUT CE QUI EST ENTRE LES BALISES ༽つ۞﹏۞༼つ SERA ENLEVÉ! N'AJOUTEZ PAS D'AUTRES ༽つ۞﹏۞༼つ
// ༽つ۞﹏۞༼つ
/**
 * Entry point to your homework. DO NOT, UNLESS TOLD BY AN INSTRUCTOR, CHANGE ANY CODE IN THIS
 * FUNCTION. DOING SO WILL GIVE YOU THE GRADE 0.
 * @return exit code
 */
int main(int argc, char **argv) {
    /*
     *  Example of an argument string you can use for test/debug:
     *  ABCD5AB5CD5A9B9CDABCD
     *
     *  Letters are tasks
     *  Numbers are delays
     *
     */
    if (argc < 2) {
        printf("Missing / Wrong arguments.\n");
        return EXIT_FAILURE;
    }

    // Start threads
    blocking_q *sched_q = malloc(sizeof(blocking_q));

    if (NULL == sched_q || !blocking_q_init(sched_q)) {
        return EXIT_FAILURE;
    }

    pthread_t sched_thread;
    pthread_t processor_threads[PROCESSOR_COUNT];
    processor processors[PROCESSOR_COUNT];

    sched_data data;
    data.sched_q = sched_q;
    data.processors = processors;

    if (0 != pthread_create(&sched_thread, NULL, scheduler, (void *) &data)) {
        return EXIT_FAILURE;
    }

    long start = time(NULL);
    for (int i = 0; i < PROCESSOR_COUNT; ++i) {

        if (!processor_init(i, processors + i)) {
            return EXIT_FAILURE;
        }

        if (0 != pthread_create(processor_threads + i,
                                NULL,
                                processor_run,
                                (void *) (processors + i))) {

            return EXIT_FAILURE;
        }
    }

    char *tasks_and_times = argv[1];

    // Fill the task queue
    unsigned long task_c = strlen(tasks_and_times);

    size_t total_sz = sizeof(task) * task_c;
    task_ptr tasks = (task_ptr) malloc(total_sz);
    memset(tasks, 0, total_sz);

    for (unsigned long i = 0; i < task_c; ++i) {
        char task_type = tasks_and_times[i];

        switch (task_type) {
            case 'A':
            case 'B':
            case 'C':
            case 'D': {
                task_ptr t = tasks + i;
                t->type = task_type;
                t->start = t->end = 0;
                blocking_q_put(sched_q, t);
                break;
            }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                sleep(task_type - '0');
                break;
            default:
                break;
        }
    }

    task_ptr poison_pill_task = malloc(sizeof(task));
    poison_pill_task->type = POISON_PILL;

    blocking_q_put(sched_q, poison_pill_task);

    pthread_join(sched_thread, NULL);

    blocking_q_destroy(sched_q);

    printf("\n\n");

    for (int i = 0; i < PROCESSOR_COUNT; ++i) {
        pthread_join(processor_threads[i], NULL);

        processor *p = processors + i;
        printf("Processor %d: Real T: %ld Work T: %ld Wait T: %ld\n",
               i,
               p->real_t,
               p->work_t,
               p->wait_t);
    }

    long end = time(NULL);
    long elapsed = end - start;

    printf("Elapsed: %ld\n", elapsed);

    free(poison_pill_task);

    for (int i = 0; i < PROCESSOR_COUNT; ++i) {
        processor *p = processors + i;
        processor_destroy(p);
    }

    free(tasks);
    free(sched_q);

    return EXIT_SUCCESS;
}
// ༽つ۞﹏۞༼つ
