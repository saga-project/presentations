//  Copyright (c) 2005-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cassert>
#include <saga/saga.hpp>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace fs = boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
// Start this example by providing an arbitrary number of hosts on the command 
// line. It will re-spawn itself on each of the hosts. Each instance will 
// increment a number stored in a central result store.

///////////////////////////////////////////////////////////////////////////////
// place in advert to store result to
#define RESULT_STORE  "advert://macpro01.cct.lsu.edu/ADSSS09/result_ex_3"   
#define SAGA_LOCATION "/usr/local/saga/"

// The overall result will be stored in the advert service using the key above
// but with the user id (`id -u`) and the process id appended. So the overall
// advertkey will be: RESULT_STORE/<uid>/<pid>, where pid is the process id of 
// the first process.
 
///////////////////////////////////////////////////////////////////////////////
// implemented somewhere else 
std::string deploy_me(std::string path, std::string target_host);
std::string get_advert_result_store(std::string basepath);

///////////////////////////////////////////////////////////////////////////////
// Helper function creating a valid host url
//
// The issue is that some SAGA API calls take saga::url's, which are implicitely
// creatable from a std::string. But if a saga::url gets created from a string
// not containing any hostname or scheme (such as 'localhost') this will result
// in an url holding this string as its path component - not exactly what we 
// need... 
//
std::string hosturl(std::string host)
{
    saga::url hosturl(host);
    if (hosturl.get_host().empty())
        hosturl.set_host(host);
    if (hosturl.get_scheme().empty())
        hosturl.set_scheme("any");
    hosturl.set_path("");
    return hosturl.get_string();
}
 
///////////////////////////////////////////////////////////////////////////////
// retrieve the current value from the advert (result store)
bool get_result(int& result, std::string advertkey)
{
    result = 0;
    try {
        saga::advert::entry e(advertkey, 
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
bool set_result(int result, std::string advertkey)
{
    try {
        saga::advert::entry e(advertkey, 
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
void respawn(std::string exename, int argc, char *argv[], std::string advertkey)
{
    assert(argc > 0);     // we shouldn't end up here without any given hosts
    try {
        // deploy this executable
        std::string targetdir = deploy_me(exename, argv[0]);

        // compose the command line
        std::vector<std::string> arguments; 

        // pass advertkey
        arguments.push_back("-c");
        arguments.push_back(advertkey);

        // pass remaining hostnames (skip first argument)
        for (int i = 1; i < argc; ++i) 
            arguments.push_back(argv[i]);

        // compose environment variables
        std::vector<std::string> environment; 
        environment.push_back("SAGA_LOCATION=" SAGA_LOCATION);
        environment.push_back("LD_LIBRARY_PATH=" SAGA_LOCATION "/lib:$LD_LIBRARY_PATH");

        // full name (path) of the executable
        fs::path executable(targetdir);
        executable /= fs::path(exename).filename();

        // create and fill job description
        saga::job::description jd;
        jd.set_attribute (saga::job::attributes::description_executable, 
            executable.string());
        jd.set_attribute (saga::job::attributes::description_working_directory, targetdir);
        jd.set_attribute (saga::job::attributes::description_interactive, saga::attributes::common_true);
        jd.set_vector_attribute (saga::job::attributes::description_arguments, arguments);
        jd.set_vector_attribute (saga::job::attributes::description_environment, environment);

        // run the job on host given by first argument
        saga::job::ostream in;
        saga::job::istream out, err;

        saga::job::service js (hosturl(argv[0]));    // job needs to run on host given by 1st argument
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
    int first_host_arg = 1;
    std::string advertkey;
    if (argc > 1 && std::string(argv[1]) == "-c") {
    // got advert store key passed from parent
        advertkey = argv[2];
        first_host_arg = 3;
    }
    else {
        advertkey = get_advert_result_store(RESULT_STORE);
        std::cout << "Look here for the overall result: " << advertkey << std::endl;
    }

    if (argc == first_host_arg) {
    // no more hosts are given, we're done!
        int result = 0;
        if (get_result(result, advertkey))
            std::cout << "The overall result is: " << result << std::endl;
    }
    else {
    // otherwise get current value, increment it, and store new value
        int result = 0;
        get_result(result, advertkey);   // ignore errors, will set result to zero

        // re-spawn this job, increment result
        if (set_result(result + 1, advertkey)) {
            // exename, numhosts, hosts, advertkey
            respawn(argv[0], argc-first_host_arg, &argv[first_host_arg], advertkey);
        }
    }
    return 0;
}

