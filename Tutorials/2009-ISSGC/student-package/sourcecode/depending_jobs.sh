#!/bin/sh
saga-advert remove_entry advert://issgc-ui//issgc10/result_ex_2
./depending_jobs  ssh://user@134.59.140.163 ssh://user@134.59.140.164
saga-advert retrieve_string advert://issgc-ui//issgc10/result_ex_2