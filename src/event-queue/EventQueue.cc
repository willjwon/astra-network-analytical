/******************************************************************************
Copyright (c) 2020 Georgia Institute of Technology
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author : William Won (william.won@gatech.edu)
*******************************************************************************/

#include "EventQueue.hh"

void AnalyticalBackend::EventQueue::add_event(AstraSim::timespec_t time_stamp, void (*fun_ptr)(void *), void *fun_arg) noexcept {
    // Event Queue is ordered by time_stamp in ascending order.
    // 1. Search Event queue:
    //      (1) if time_stamp is smaller, search next entry
    //      (2) if time_stamp is equal, add event to that entry
    //      (3) if time_stamp is larger, it means no entry matches time_stamp
    //            -> insert new event queue element

    // should assign event that happens later than current_time
    assert(EventQueueEntry::compare_time_stamp(current_time, time_stamp) < 0);

    for (auto it = event_queue.begin(); it != event_queue.end(); it++) {
        auto time_stamp_compare_result = EventQueueEntry::compare_time_stamp(it->get_time_stamp(), time_stamp);
        // if time_stamp is smaller, do nothing
        if (time_stamp_compare_result == 0) {
            // equal time_stamp -> insert event here
            it->add_event(fun_ptr, fun_arg);
            return;
        } else if (time_stamp_compare_result > 0) {
            // entry's time stamp is larger -> no matching queue entry found
            // insert new queue entry
            auto new_queue_entry = event_queue.emplace(it, time_stamp);
            new_queue_entry->add_event(fun_ptr, fun_arg);
            return;
        }
    }

    // flow falls here when
    // (1) event queue was empty
    // (2) given time_stamp is larger than largest entry
    //      -> for both cases, create new entry at the end of the event_queue
    event_queue.emplace_back(time_stamp);
    event_queue.back().add_event(fun_ptr, fun_arg);
}

AstraSim::timespec_t AnalyticalBackend::EventQueue::get_current_time() const noexcept {
    return current_time;
}

void AnalyticalBackend::EventQueue::proceed() noexcept {
    auto& event_queue_entry = event_queue.front();

    // proceed current time
    current_time = event_queue_entry.get_time_stamp();

    // run events
    event_queue_entry.run_events();

    // remove queue entry
    event_queue.pop_front();
}

bool AnalyticalBackend::EventQueue::empty() const noexcept {
    return event_queue.empty();
}

void AnalyticalBackend::EventQueue::print() const noexcept {
    std::cout << "===== event-queue =====" << std::endl;
    std::cout << "CurrentTime: " << current_time.time_val << std::endl << std::endl;
    for (const auto& event_queue_entry : event_queue) {
        event_queue_entry.print();
    }
    std::cout << "======================" << std::endl << std::endl;
}
