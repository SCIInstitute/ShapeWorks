#pragma once

//TODO: Figure out memory logging in Windows or get rid of this
// Disabled by default because this code is *nix specific.
#define LOG_MEMORY_USAGE

#ifdef LOG_MEMORY_USAGE
void process_mem_usage(double& vm_usage, double& resident_set);
#endif LOG_MEMORY_USAGE
