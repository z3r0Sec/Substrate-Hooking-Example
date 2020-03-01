#include <pthread.h>
#include <jni.h>
#include <memory.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>

#include "Includes/Logger.h"
#include "Substrate/CydiaSubstrate.h"
#import "Includes/Utils.h"

bool PlayerUpdateHookInitialized = false;
const char* libName = "libil2cpp.so";

void(*old_Player_Update)(void *instance);
void Player_Update(void *instance) {
    //Check if instance is a nullptr to prevent crashes!  If the instance object is a nullptr, this is what the call to update would look like in C++: 
    //NULL.Update(); and dat doesnt make sense right?
    if(instance != NULL) {
        if(!PlayerUpdateHookInitialized){
            PlayerUpdateHookInitialized = true;
            LOGI("Player_Update hooked");
        }
    }
    old_Player_Update(instance);
}


void* hack_thread(void*) {
    do {
        sleep(1);
    } while (!isLibraryLoaded(libName));
    LOGI("I found the il2cpp lib. Address is: %lu", findLibrary(libName));
    LOGI("Hooking Player_Update");
    MSHookFunction((void*)getAbsoluteAddress(libName, 0xF09F00), (void*)Player_Update, (void**) &old_Player_Update);

    return NULL;
}

__attribute__((constructor))
void libhook_main() {
    LOGI("I have been loaded. Mwuahahahaha");
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}