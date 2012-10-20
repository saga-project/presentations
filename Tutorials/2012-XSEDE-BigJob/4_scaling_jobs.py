""" Example application demonstrating how to submit a jobs with PilotJob.  """

import os
import time
import sys
from pilot import PilotComputeService, ComputeDataService, State
COORDINATION_URL = "redis://ILikeBigJob_wITH-REdIS@gw68.quarry.iu.teragrid.org:6379"
	

### This is the number of jobs you want to run
NUMBER_JOBS=24

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

    pilot_compute_description.append({ "service_url": "ssh://ranger",
                                       "number_of_processes": 16,
                                       "allocation": "TG-MCB090174",
                                       "queue": "normal",
                                       "processes_per_node":16,
                                       "working_directory": "/share/home/01539/pmantha/agent",
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
                # When multiple machines considered executable location might vary.
                # User has to to create link to executable in $HOME directory
                # In below case testexe is a link to executable echo on both Ranger and Lonstar
                "executable": "$HOME/testexe",
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
