#include "ml_thread.h"
#include <stdio.h>

void* consumer(void* arg) {
    /**
    * TODO: Consumer thread logic.
    * * Each consumer is responsible for:
    * 1. Safely retrieving a task from the shared circular buffer.
    * 2. Checking for the termination signal (is_last flag).
    * 3. Performing matrix-vector multiplication.
    * 4. Finding the predicted class.
    * 5. Storing the result in the global 'predictions' array.
    * * Refer to the Lab Writeup for synchronization details and the 
    * mathematical definition of the inference task.
    */

    // --- YOUR CODE STARTS HERE ---
    while(1){
        pthread_mutex_lock(&mutex);
        while(count == 0){
            pthread_cond_wait(&not_empty, &mutex); 
        }
        InferenceTask task = queue[head];
        head = (head + 1) % BUFFER_SIZE;
        count--;
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        if(task.is_last){
            pthread_exit(NULL);
        }
        float output[NUM_CLASSES] = {0};
        for(int j = 0; j < NUM_CLASSES; j++){
            for(int k = 0; k < IMG_SIZE; k++){
                output[j] += task.pixels[k] * weights[k][j];
            }
        }
        float highest = output[0];
        int highest_idx = 0;
        for(int i = 1; i < 10; i++){
            if(highest < output[i]){
                highest = output[i];
                highest_idx = i;
            }
        }
        predictions[task.image_id] = highest_idx;
    }
    // --- YOUR CODE ENDS HERE ---


    return NULL;
}