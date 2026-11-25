#ifndef BASE_THREAD_LINUX_H
#define BASE_THREAD_LINUX_H

//- os impementations for threading
// note: keeping the implementation linux specific for now

internal void mutex_create();
internal void mutex_destroy();
internal void mutex_lock();
internal void mutex_unlock();

#endif
