#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
 
// 뮤텍스 객체 선언
pthread_mutex_t mutex_lock;

enum { STATE_1, STATE_2 } state = STATE_1;

pthread_cond_t      condA  = PTHREAD_COND_INITIALIZER;
pthread_cond_t      condB  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
 
void *t_function1(void *data) {

    int i=0;
    char* thread_name = (char*)data;
    
    while(i <= 11)
    {
        /* Wait for state 1 */
        pthread_mutex_lock(&mutex);
        while (state != STATE_1)
            pthread_cond_wait(&condA, &mutex);
        pthread_mutex_unlock(&mutex);

        // 3이하, 8~10일 때 출력
        if((0 <= i && i <= 3) || (8  <= i && i <= 10)) {
            printf("%s count value:%d\n", thread_name, i);
            //sleep(1);
        }

        /* Set state to 2 and wake up thread 2 */
        pthread_mutex_lock(&mutex);
        state = STATE_2;
        pthread_cond_signal(&condB);
        pthread_mutex_unlock(&mutex);

        i++;
    }
   
}

void *t_function2(void *data) {
    int n = 0, rValue;
    char* thread_name = (char*)data;

    while(n<=11) {
        /* Wait for state 2 */
        pthread_mutex_lock(&mutex);
        while (state != STATE_2)
            pthread_cond_wait(&condB, &mutex);
        pthread_mutex_unlock(&mutex);
        
        if(!((0 <= n && n <= 3) || (8 <= n && n <= 10))) {
            printf("%s count value:%d\n", thread_name, n);
            //sleep(1);
        }

        /* Set state to 1 and wake up thread 1*/
        pthread_mutex_lock(&mutex);
        state = STATE_1;
        pthread_cond_signal(&condA);
        pthread_mutex_unlock(&mutex);

        n++;
    }

    
}

int main()
{
    pthread_t p_thread1, p_thread2;
    int status;

    // 뮤텍스 객체 초기화, 기본 특성으로 초기화
    pthread_mutex_init(&mutex_lock, NULL);
    
    // The new thread starts execution by invoking t_function
    pthread_create(&p_thread1, NULL, t_function1, (void *)"By function1,"); 
    pthread_create(&p_thread2, NULL, t_function2, (void *)"By function2,"); 
    
    // pthread1이 종료되는 것을 기다린다.
    pthread_join(p_thread1, NULL);
    pthread_join(p_thread2, NULL);
    
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condA);
    pthread_cond_destroy(&condB);
    
    return 0;
}
