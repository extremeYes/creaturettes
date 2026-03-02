#include <windows.h>
#include <stdio.h>

// 1. Windows thread signature: returns DWORD, takes LPVOID (void*)
DWORD WINAPI my_worker(LPVOID arg) {
    int my_id = *(int*)arg;

    printf("Thread %d starting!\n", my_id);
    Sleep(1000); // Windows Sleep is in milliseconds
    printf("Thread %d finished!\n", my_id);

    return 0;
}

int main() {
    // pthread_t becomes HANDLE
    HANDLE t1, t2;
    int ids[2] = {1, 2};

    // 2. Spawn: CreateThread
    // Args: Security(NULL), StackSize(0=default), Function, Arg, Flags(0=run immediately), &ThreadId(NULL)
    t1 = CreateThread(NULL, 0, my_worker, &ids[0], 0, NULL);
    t2 = CreateThread(NULL, 0, my_worker, &ids[1], 0, NULL);

    printf("Main thread waiting...\n");

    // 3. Wait: WaitForSingleObject (pthread_join equivalent)
    // INFINITE means block until it finishes.
    WaitForSingleObject(t1, INFINITE);
    WaitForSingleObject(t2, INFINITE);

    // 4. Cleanup: Windows requires you to close handles to avoid memory leaks
    CloseHandle(t1);
    CloseHandle(t2);

    printf("Main thread exiting safely.\n");
    return 0;
}
