

#include <cstdlib>	// rand

#include "groupWork.h"
#include "worker.h"
#include "../../platform/logger.h"
#include "../../platform/mailbox.h"

namespace {

Mailbox* myOutMailbox;
Worker* localWorker;

void sendWork() {
	if (myOutMailbox->isMail() ){
		// My last mail didn't go out yet
		log("Mail still in mailbox\n");
	}
	else {
		myOutMailbox->put(33);	// 33 dummy data
		log("App put work\n");
	}
}

} // namespace



void GroupWork::init(Mailbox* aMailbox, Worker* aLocalWorker){
	myOutMailbox = aMailbox;
	localWorker = aLocalWorker;
}

void GroupWork::initiateGroupWork() {
	localWorker->work();
	sendWork();		// tell group to work
}

void GroupWork::randomlyInitiateGroupWork() {
	if (rand() % 10 == 1) {
		initiateGroupWork();
	}
}

void GroupWork::workInIsolation() {
	// work on this unit, but don't tell group
	localWorker->work();
}
