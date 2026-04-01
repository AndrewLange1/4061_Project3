#include "ml_thread.h"
#include <stdio.h>
#include <stdlib.h>

void* producer(void* arg) {
    int num_workers = *(int*)arg;
    /**
    * TODO: Producer thread logic.
    * * The producer is responsible for:
    * 1. Opening the image and label binary files.
    * 2. Reading image data and labels into InferenceTask structures.
    * 3. Safely adding tasks to the shared circular buffer (handling full buffer).
    * 4. Sending a termination signal to each worker thread.
    * * Please refer to the Lab Writeup for details on circular buffer management 
    * and thread synchronization using mutex/condition variables.
    */

    // --- YOUR CODE STARTS HERE ---

    void* producer(void* arg) {
        int num_workers = *(int*)arg;

        FILE *imgf = fopen("../data/images.bin", "rb");
        FILE *labf = fopen("../data/labels.bin", "rb");
        if (imgf == NULL || labf == NULL) {
            perror("fopen");
            if (imgf) fclose(imgf);
            if (labf) fclose(labf);
            return NULL;
        }

        for (int i = 0; i < TOTAL_IMAGES; i++) {
            InferenceTask task;

            /* fill in real task fields here */
            task.index = i;
            task.is_last = 0;

            if (fread(task.image, sizeof(float), IMG_SIZE, imgf) != IMG_SIZE) {
                fprintf(stderr, "Failed to read image %d\n", i);
                break;
            }

            if (fread(&task.label, sizeof(int), 1, labf) != 1) {
                fprintf(stderr, "Failed to read label %d\n", i);
                break;
            }

            ground_truth[i] = task.label;

            pthread_mutex_lock(&mutex);

            while (count == BUFFER_SIZE) {
                pthread_cond_wait(&not_full, &mutex);
            }

            queue[tail] = task;
            tail = (tail + 1) % BUFFER_SIZE;
            count++;

            pthread_cond_signal(&not_empty);
            pthread_mutex_unlock(&mutex);
        }

        for (int i = 0; i < num_workers; i++) {
            InferenceTask task;

            /* set only the termination marker field(s) */
            task.is_last = 1;

            pthread_mutex_lock(&mutex);

            while (count == BUFFER_SIZE) {
                pthread_cond_wait(&not_full, &mutex);
            }

            queue[tail] = task;
            tail = (tail + 1) % BUFFER_SIZE;
            count++;

            pthread_cond_signal(&not_empty);
            pthread_mutex_unlock(&mutex);
        }

        fclose(imgf);
        fclose(labf);
        return NULL;
    }

    // --- YOUR CODE ENDS HERE ---


    return NULL;
}