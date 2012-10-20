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

    print ("Finished Pilot-Job setup. Submit compute units")
    # submit Set A compute units
    for i in range(NUMBER_JOBS):
        compute_unit_description = {
                "executable": "/bin/echo",
                "arguments": ["Hello","$ENV1","$ENV2"],
                "environment": ['ENV1=env_arg1','ENV2=env_arg2'],
                "total_cpu_count": 4,            
                "spmd_variation":"mpi",
                "output": "A_stdout.txt",
                "error": "A_stderr.txt",
                }    
        compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)

    # Chaining tasks i.e submit a compute unit, when compute unit from A is successfully executed.

    # Get all compute units
    all_A_cus=compute_data_service.get_all_cus()
    temp = list(all_A_cus)
    while 1:
        temp2 = list(temp)
        if len(temp2) == 0:
            break
        for i in range(len(temp2)):
            if temp2[i].get_state() == "Done":
                compute_unit_description = {
                    "executable": "/bin/echo",
                    "arguments": ["$ENV1","$ENV2"],
                    "environment": ['ENV1=task_B'+str(i),'ENV2=after_task_A'+str(i)],
                    "total_cpu_count": 1,
                    "spmd_variation":"single",
                    "output": "B_stdout.txt",
                    "error": "B_stderr.txt"
                }
                compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)
                del temp[i]
        

    # Again chaining tasks i.e submit a compute unit, when compute unit from B is successfully executed.
    allcus=compute_data_service.get_all_cus()
    # Remove CUs of A from total list of CUs to get only B
    for i in all_A_cus:
        allcus.remove(i)   
    all_B_cus = list(allcus) 

    # Submit a Compute Unit, when Compute Unit from B is successfully executed.
    while 1:
        temp = list(all_B_cus)
        if len(temp) == 0:
            break
        for i in range(len(temp)):
            if temp[i].get_state() == "Done":
                compute_unit_description = {
                    "executable": "/bin/echo",
                    "arguments": ["$ENV1","$ENV2"],
                    "environment": ['ENV1=task_C'+str(i),'ENV2=after_task_B'+str(i)],
                    "total_cpu_count": 1,
                    "spmd_variation":"single",
                    "output": "C_stdout.txt",
                    "error": "C_stderr.txt"
                }
                compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)
                del all_B_cus[i]


    print ("Terminate Pilot Jobs")
    compute_data_service.cancel()    
    pilot_compute_service.cancel()
    end_time=time.time()
    print "Total time to solution-" + str(round(end_time-start_time,2))
