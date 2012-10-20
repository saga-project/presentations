""" Example application demonstrating how to submit a jobs with PilotJob.  """

import os
import time
import sys
from pilot import PilotComputeService, ComputeDataService, State
	
### This is the number of jobs you want to run
NUMBER_JOBS=4
COORDINATION_URL = "redis://ILikeBigJob_wITH-REdIS@gw68.quarry.iu.teragrid.org:6379"

if __name__ == "__main__":

    start_time=time.time()
    pilot_compute_service = PilotComputeService(COORDINATION_URL)
    pilot_compute_description=[]

    pilot_compute_description.append({ "service_url": "ssh://localhost",
                                       "number_of_processes": 12,
                                       "allocation": "TG-MCB090174",
	                               "queue": "normal",
                                       "processes_per_node":12,
                                       "working_directory": os.getcwd()+"/agent",
                                       "walltime":10,
                                     })

    for pcd in pilot_compute_description:
        pilot_compute_service.create_pilot(pilot_compute_description=pcd)

    compute_data_service = ComputeDataService()
    compute_data_service.add_pilot_compute_service(pilot_compute_service)

    print ("Finished Pilot-Job setup. Submitting compute units")

    # submit compute units
    for i in range(NUMBER_JOBS):
        compute_unit_description = {
                "executable": "/bin/echo",
                "arguments": ["Hello","$ENV1","$ENV2"],
                "environment": ['ENV1=env_arg1','ENV2=env_arg2'],
                "total_cpu_count": 4,            
                "spmd_variation":"mpi",
                "output": "stdout.txt",
                "error": "stderr.txt",
                }    
        compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)

    print ("Waiting for compute units to complete")
    compute_data_service.wait()

    print ("Terminate Pilot Jobs")
    compute_data_service.cancel()    
    pilot_compute_service.cancel()
    end_time=time.time()

    print "Total time to solution-" + str(round(end_time-start_time,2))
