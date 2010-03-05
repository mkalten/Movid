//
// moPipeline.cpp
//
// Handle a group of object (pipeline)
//

#include <assert.h>
#include "moPipeline.h"
#include "moLog.h"

LOG_DECLARE("Pipeline");

MODULE_DECLARE_EX(Pipeline,, "native", "Handle object list");

moPipeline::moPipeline() : moModule(MO_MODULE_NONE, 0, 0) {
	MODULE_INIT();
}

moPipeline::~moPipeline() {
	std::vector<moModule *>::iterator it = this->modules.begin();
	while ( it != this->modules.end() ) {
		delete *it;
		this->modules.erase(it);
	}
}

moModule *moPipeline::firstModule() {
	assert( this->modules.size() > 0 );
	return this->modules[0];
}

moModule *moPipeline::lastModule() {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1];
}

void moPipeline::addElement(moModule *module) {
	assert( module != NULL );
	LOG(TRACE) << "add <" << module->property("id").asString() << "> to <" \
		<< this->property("id").asString() << ">";
	module->owner = this;
	this->modules.push_back(module);
}

void moPipeline::removeElement(moModule *module) {
	std::vector<moModule *>::iterator it;
	LOG(TRACE) << "remove <" << module->property("id").asString() << "> from <" \
		<< this->property("id").asString() << ">";
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( *it == module ) {
			this->modules.erase(it);
			break;
		}
	}
}

void moPipeline::setInput(moDataStream* stream, int n) {
	this->firstModule()->setInput(stream, n);
}

moDataStream* moPipeline::getInput(int n) {
	return this->lastModule()->getInput(n);
}

moDataStream* moPipeline::getOutput(int n) {
	return this->lastModule()->getOutput(n);
}

int moPipeline::getInputCount() {
	return this->firstModule()->getInputCount();
}

int moPipeline::getOutputCount() {
	return this->lastModule()->getOutputCount();
}

moDataStreamInfo *moPipeline::getInputInfos(int n) {
	return this->firstModule()->getInputInfos(n);
}

moDataStreamInfo *moPipeline::getOutputInfos(int n) {
	return this->lastModule()->getOutputInfos(n);
}

void moPipeline::start() {
	std::vector<moModule *>::iterator it;

	moModule::start();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->start();
	}
}

void moPipeline::stop() {
	std::vector<moModule *>::iterator it;

	moModule::stop();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->stop();
	}
}

void moPipeline::update() {
	// nothing done in pipeline
	return;
}

void moPipeline::poll() {
	std::vector<moModule *>::iterator it;

	LOGM(TRACE) << "poll";

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->poll();
	}
}

unsigned int moPipeline::size() {
	return this->modules.size();
}

moModule *moPipeline::getModule(unsigned int index) {
	assert( index >= 0 );
	assert( index < this->size() );

	return this->modules[index];
}

void moPipeline::setGroup(bool group) {
	this->is_group = group;
}

bool moPipeline::isGroup() {
	return this->is_group;
}

bool moPipeline::isPipeline() {
	return true;
}

bool moPipeline::haveError() {
	std::vector<moModule *>::iterator it;
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( (*it)->haveError() )
			return true;
	}
	return false;
}

std::string moPipeline::getLastError() {
	std::vector<moModule *>::iterator it;
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( (*it)->haveError() )
			return (*it)->getLastError();
	}
	return "";
}
