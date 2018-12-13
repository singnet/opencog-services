#ifndef _OPENCOGSERVICES_SERVICE_IMPL_H
#define _OPENCOGSERVICES_SERVICE_IMPL_H

#include "opencogServicesCore.h"
#include "GuileSessionManager.h"
#include "OpencogSNETService.h"
#include "OpencogSNETServiceFactory.h"

namespace opencogservices
{

class ServiceImpl final : public ServiceDefinition::Service
{
public:
	ServiceImpl(GuileSessionManager &sessionManager);
	Status execute(ServerContext *context, const Command *input, CommandOutput *output) override;
	Status asynchronousTask(ServerContext *context, const Command *input, Ticket *ticket) override;
	void exec_service(ServerContext *context, const Command *input, CommandOutput *output);
	void threadJobManager(ServerContext *context, const Command *input, const string &fname);

private:
	GuileSessionManager *_sessionManager;
};
} // namespace opencogservices

#endif