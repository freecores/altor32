//-----------------------------------------------------------------------------
//                                     AltOR32 
//                         Alternative Lightweight OpenRISC 
//                                Ultra-Embedded.com
//                               Copyright 2011 - 2012
//
//                         Email: admin@ultra-embedded.com
//
//                                License: GPL
//  Please contact the above address if you would like a version of this 
//  software with a more permissive license for use in closed source commercial 
//  applications.
//-----------------------------------------------------------------------------
//
// This file is part of AltOR32 Alternative Lightweight OpenRISC project.
//
// AltOR32 is free software; you can redistribute it and/or modify it under 
// the terms of the GNU General Public License as published by the Free Software 
// Foundation; either version 2 of the License, or (at your option) any later 
// version.
//
// AltOR32 is distributed in the hope that it will be useful, but WITHOUT ANY 
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public License
// along with AltOR32; if not, write to the Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//-----------------------------------------------------------------------------
#include "rtos.h"
#include "mutex.h"
#include "thread.h"
#include "os_assert.h"

#ifdef INCLUDE_MUTEX
//-----------------------------------------------------------------
// mutex_init: Initialise mutex
//-----------------------------------------------------------------
void mutex_init(struct mutex *pMtx)
{
    OS_ASSERT(pMtx != NULL);

    // No default owner
    pMtx->owner = NULL;

    // Pending thread list
    list_init(&pMtx->pend_list);
}
//-----------------------------------------------------------------
// mutex_acquire: Acquire mutex (non-recursive)
//-----------------------------------------------------------------
void mutex_acquire(struct mutex *pMtx)
{
    struct thread* this_thread;
    int cr;

    OS_ASSERT(pMtx != NULL);

    cr = critical_start();

    // Get current (this) thread
    this_thread = thread_current();

    OS_ASSERT(pMtx->owner != this_thread);

    // Is the mutex not already locked
    if (pMtx->owner == NULL)
    {
        // Acquire mutex for this thread
        pMtx->owner = this_thread;
    }
    // The mutex is already 'owned', add
    // thread to pending list
    else
    {
        struct link_node *listnode;

        // Get list node
        listnode = &this_thread->blocking_node;

        // Add node to end of pending list
        list_insert_last(&pMtx->pend_list, listnode);

        // Block the thread from running
        thread_block(this_thread);
    }

    critical_end(cr);
}
//-----------------------------------------------------------------
// mutex_release: Release mutex (non-recursive)
//-----------------------------------------------------------------
void mutex_release(struct mutex *pMtx)
{
    struct thread* this_thread;
    int cr;

    OS_ASSERT(pMtx != NULL);

    cr = critical_start();

    // Get current (this) thread
    this_thread = thread_current();

    // We cannot release a lock that we dont own!
    OS_ASSERT(this_thread == pMtx->owner);

    // If there are threads pending on this mutex
    if (!list_is_empty(&pMtx->pend_list))
    {
        // Unblock the first pending thread
        struct link_node *node = list_first(&pMtx->pend_list);

        // Get the thread item
        struct thread* thread = list_entry(node, struct thread, blocking_node);

        // Remove node from linked list
        list_remove(&pMtx->pend_list, node);

        // Transfer mutex ownership
        pMtx->owner = thread;

        // Unblock the first waiting thread
        thread_unblock(thread);
    }
    // Else no-one wants it, no owner
    else
        pMtx->owner = NULL;

    critical_end(cr);
}
#endif