#pragma once
#include "device.h"

typedef struct cmd_buffer
{
	VkCommandBuffer		buffer;
	VkFence				fence;
} cmd_buffer;

typedef struct cmd_chain 
{
	VkCommandPool 		pool;
	VkCommandBuffer* 	buffers;
	VkFence*			fences;
	uint 				nbuffers;
	uint 				idx;
} cmd_chain;

cmd_buffer* request_cmd();

void submit_cmd(cmd_buffer* cmd, queueID q);

void init_cmd_pools();

void create_cmd_chain(uint nbuffers, cmd_chain* cmd);

void cmd_chain_begin(cmd_chain* cmd, uint idx);

void cmd_chain_end(cmd_chain* cmd, queueID q, VkSubmitInfo* info);

void destroy_cmd_chain(cmd_chain* cmd);