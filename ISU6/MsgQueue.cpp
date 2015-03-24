#include "MsgQueue.h"

MsgQueue::MsgQueue(unsigned long maxSize) {
	this->maxSize = maxSize;
}

void MsgQueue::send(unsigned long id, Message * msg) {
	// Take mutex, because we're playing with mqitems.
	pthread_mutex_lock(&mqMut);
	// Check if full
	while (mqitems.size() >= maxSize) {
		pthread_cond_wait(&receiver, &mqMut);
	}

	// Put item into the back of the queue.
	Item item;
	item.id = id;
	item.msg = msg;
	mqitems.push_back(item);

	// Signal that an item has been put in the queue.
	pthread_cond_signal(&sender);

	// Release mutex
	pthread_mutex_unlock(&mqMut);
}
Message * MsgQueue::receive(unsigned long & id) {
	// Lock mutex to use mqitems.
	pthread_mutex_lock(&mqMut);
	while (mqitems.empty()) {
		pthread_cond_wait(&sender, &mqMut);
	}

	// Receive first item in queue.
	Item item = mqitems.front();
	// Remove first item in queue.
	mqitems.pop_front();
	// Signal that an item has been removed.
	pthread_cond_signal(&receiver);
	// Unlock mutex
	pthread_mutex_unlock(&mqMut);
	// Return id & msg
	id = item.id;
	return item.msg;
}

MsgQueue::~MsgQueue() {
	// Destruction is vital!.
}
