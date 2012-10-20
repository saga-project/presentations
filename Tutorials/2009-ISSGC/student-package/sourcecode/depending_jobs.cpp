//  Copyright (c) 2005-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cassert>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>

///////////////////////////////////////////////////////////////////////////////
// Start this example by providing an arbitrary number of hosts on the command 
// line. It will re-spawn itself on each of the hosts. Each instance will 
// increment a number stored in a central result store.

///////////////////////////////////////////////////////////////////////////////
#define RESULT_STORE  "advert://issgc-ui//issgc10/result_ex_2"   // place in advert to store result to
#define JOB_PATH      "/tmp/saga/depending_jobs"                 // put the correct path here
#define SAGA_LOCATION "/opt/saga-1.3.1-issgc"

///////////////////////////////////////////////////////////////////////////////
// retrieve the current value from the advert (result store)
bool get_result(int& result)
{
    result = 0;
    try {
        saga::advert::entry e(RESULT_STORE, 
            saga::advert::CreateParents | saga::advert::Create | saga::advert::Read);
        result = boost::lexical_cast<int>(e.retrieve_string());
    }
    catch (saga::exception const& e) {
        if (e.get_error() != saga::NoSuccess)
            std::cerr << "saga::exception caught: " << e.what () << std::endl;
        return false;
    }
    catch (std::exception const& e) {
        std::cerr << "std::exception caught: " << e.what () << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "unexpected exception caught" << std::endl;
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// store the current value into the advert (result store)
bool set_result(int result)
{
    try {
        saga::advert::entry e(RESULT_STORE, 
            saga::advert::CreateParents | saga::advert::Create | saga::advert::ReadWrite);
        e.store_string(boost::lexical_cast<std::string>(result));
    }
    catch (saga::exception const& e) {
        std::cerr << "saga::exception caught: " << e.what () << std::endl;
        return false;
    }
    catch (std::exception const& e) {
        std::cerr << "std::exception caught: " << e.what () << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "unexpected exception caught" << std::endl;
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// the routine spawning the SAGA jobs and waiting for their results
void respawn(int argc, char *argv[])
{
    assert(argc > 1);     // we shouldn't end up here without any given hosts
    try {

        // compose the command line, skip first argument
        std::vector<std::string> arguments; 
        for (int i = 2; i < argc; ++i) 
            arguments.push_back(argv[i]);

        // compose environment variables
        std::vector<std::string> environment; 
        environment.push_back("SAGA_LOCATION=" SAGA_LOCATION);
        environment.push_back("LD_LIBRARY_PATH=" SAGA_LOCATION "/lib:$LD_LIBRARY_PATH");

        // create and fill job description
        saga::job::description jd;
        jd.set_attribute (saga::job::attributes::description_executable, JOB_PATH);
        jd.set_attribute (saga::job::attributes::description_interactive, saga::attributes::common_true);
        jd.set_vector_attribute (saga::job::attributes::description_arguments, arguments);
        jd.set_vector_attribute (saga::job::attributes::description_environment, environment);

        // run the job on host given by first argument
        saga::job::ostream in;
        saga::job::istream out, err;

        saga::job::service js (argv[1]);    // job needs to run on host given by 1st argument
        saga::job::job j = js.create_job(jd);
        in = j.get_stdin();
        out = j.get_stdout();
        err = j.get_stderr();

        j.run();

        // wait for the job to start
        saga::job::state s = j.get_state();
        while (s != saga::job::Running && s != saga::job::Failed)
            s = j.get_state();

        // if the job didn't start successfully, print error message
        if (s == saga::job::Failed) {
            std::cerr << "SAGA job: " << j.get_job_id() << " failed (state: " 
                      << saga::job::detail::get_state_name(s) << ")\n";
        }

        // wait for the job to Finish
        s = j.get_state();
        while (s == saga::job::Running)
            s = j.get_state();

    }
    catch (saga::exception const& e) {
        std::cerr << "saga::exception caught: " << e.what () << std::endl;
    }
    catch (std::exception const& e) {
        std::cerr << "std::exception caught: " << e.what () << std::endl;
    }
    catch (...) {
        std::cerr << "unexpected exception caught" << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc == 1) {
    // no more hosts are given, we're done!
        int result = 0;
        if (get_result(result))
            std::cout << "The overall result is: " << result << std::endl;
    }
    else {
    // otherwise get current value, increment it, and store new value
        int result = 0;
        get_result(result);   // ignore errors, will set result to zero

        // re-spawn this job, increment result
        if (set_result(result + 1)) 
            respawn(argc, argv);
    }
    return 0;
}
