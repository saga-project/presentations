import sys
import os
import time
import logging
from pilot import PilotComputeService, ComputeDataService, State

COORDINATION_URL = "redis://ILikeBigJob_wITH-REdIS@gw68.quarry.iu.teragrid.org:6379"

if __name__ == "__main__":      
    pilot_compute_service = PilotComputeService(COORDINATION_URL)
    pilot_compute_description = []

    # create pilot job service and initiate a pilot job
    pilot_compute_description.append({
                             "service_url": 'ssh://localhost',
                             "number_of_processes": 12,                             
                             "working_directory": os.path.join(os.getcwd(), "agent")
                            })
    pilot_compute_description.append({
                             "service_url": 'ssh://ranger',
                             "number_of_processes": 16,
                             "working_directory": "/share/home/01539/pmantha/agent"
                            })

    for pcd in pilot_compute_description:
        pilotjob = pilot_compute_service.create_pilot(pilot_compute_description=pcd)

    compute_data_service = ComputeDataService()
    compute_data_service.add_pilot_compute_service(pilot_compute_service)
    
    # Submit 24 compute units
    for i in range(24):
        compute_unit_description = {
            "executable": "/bin/cat",
            "arguments": ["test.txt"],
            "total_core_count": 1,
            "output": "stdout.txt",
            "error": "stderr.txt",   
            "file_transfer": ["ssh://" + os.path.dirname(os.path.abspath(__file__)) + "/test.txt > BIGJOB_WORK_DIR"]
        }    
        compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)


    logging.debug("Finished submission. Waiting for completion of CU")
    compute_data_service.wait()
    
    
    logging.debug("Terminate Pilot Compute Service")
    compute_data_service.cancel()    
    pilot_compute_service.cancel()
