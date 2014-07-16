// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flmsg
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef THREADS_H_
#define THREADS_H_

#include <config.h>

#include <pthread.h>

/// This ensures that a mutex is always unlocked when leaving a function or block.
class guard_lock
{
public:
	guard_lock(pthread_mutex_t* m) : mutex(m) {
		pthread_mutex_lock(mutex);
	}
	~guard_lock(void) {
		pthread_mutex_unlock(mutex);
	}
private:
	pthread_mutex_t* mutex;
};

#endif // !THREADS_H_
