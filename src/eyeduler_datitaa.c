
HANDLE my_lock;
my_lock = CreateMutex(NULL, FALSE, NULL); // Init

// Inside thread:
WaitForSingleObject(my_lock, INFINITE); // LOCK
shared_counter++;
ReleaseMutex(my_lock);                  // UNLOCK

// End of program:
CloseHandle(my_lock);
