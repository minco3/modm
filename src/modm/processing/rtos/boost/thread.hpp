/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BOOST_THREAD_HPP
#define MODM_BOOST_THREAD_HPP

#ifndef MODM_RTOS_THREAD_HPP
#	error "Don't include this file directly, use <modm/processing/rtos/thread.hpp>"
#endif

#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>

/**
 * \brief	Create a timed periodic loop
 * 
 * Example:
 * \code
 * void
 * Thread::run()
 * {
 *     TIME_LOOP(20 * MILLISECONDS)
 *     {
 *         // will be executed every 20ms
 *     }
 * }
 * \endcode
 * 
 * \param	frequency	Frequency in ticks
 * 
 * \see		MILLISECONDS
 * 
 * \hideinitializer
 * \ingroup	boost_rtos
 */
/*#define	TIME_LOOP(frequency)										\
		for(portTickType lastTime = xTaskGetTickCount() ;			\
			vTaskDelayUntil(&lastTime,								\
					static_cast<portTickType>(frequency)), true ;	\
			)*/

/**
 * \brief	Convert between scheduler ticks and milliseconds
 * 
 * For example 20 ms in scheduler ticks might be written as:
 * \code
 * 20 * MILLISECONDS
 * \endcode
 * 
 * \hideinitializer
 * \ingroup	boost_rtos
 */
#define	MILLISECONDS		1

namespace modm
{
	namespace rtos
	{
		// forward declaration
		class Scheduler;
		
		/**
		 * \brief	Thread
		 * 
		 * \ingroup	boost_rtos
		 */
		class Thread
		{
		public:
			/**
			 * \brief	Create a Thread
			 * 
			 * \param	priority	unused for boost::thread
			 * \param	stackDepth	unused for boost::thread
			 * \param	name		unused for boost::thread
			 * 
			 * \warning	Threads may not be created while the scheduler is running!
			 * 			Create them be before calling Scheduler::schedule() or
			 * 			stop the scheduler and restart it afterwards.
			 */
			Thread(uint32_t priority = 0,
					uint16_t stackDepth = 0,
					const char* name = NULL);
			
			/// Delete the thread
			virtual ~Thread();
			
			/// Obtain the priority of the thread
			uint_fast32_t
			getPriority() const
			{
				return 0;
			}
			
			/**
			 * \brief	Set the priority of the thread
			 * 
			 * Does nothing for boost::thread.
			 */
			void
			setPriority(uint_fast32_t priority)
			{
				// avoid compiler warnings
				(void) priority;
			}
			
			/**
			 * If a thread wishes to avoid being interrupted, it can create an
			 * instance of Lock. Objects of this class disable interruption
			 * for the thread that created them on construction, and restore
			 * the interruption state to whatever it was before on destruction.
			 * 
			 * \see		boost::this_thread::disable_interruption
			 */
			typedef boost::this_thread::disable_interruption Lock;
			
		protected:
			/**
			 * \brief	Delay for the number of Milliseconds
			 */
			static inline void
			sleep(uint32_t ms)
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
			}
			
			/**
			 * \brief	Force a context switch
			 * 
			 * Gives control to other threads ready to run.
			 */
			static inline void
			yield()
			{
				boost::this_thread::yield();
			}
			
			/**
			 * \brief	Worker function
			 * 
			 * Must be implemented to never return (i.e. continuous loop)
			 */
			virtual void
			run() = 0;
			
		private:
			friend class Scheduler;
			
			// start the execution of the thread
			void
			start();
			
			Thread *next;
			static Thread* head;
			
			boost::mutex mutex;
			boost::scoped_ptr<boost::thread> thread;
		};
	}
}

#endif // MODM_BOOST_THREAD_HPP