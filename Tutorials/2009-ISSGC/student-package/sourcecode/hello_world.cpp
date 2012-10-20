//  Copyright (c) 2005-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <saga/saga.hpp>
#include <boost/thread.hpp>
#include <boost/version.hpp>

#if BOOST_VERSION < 103600
#error "This code is usable with Boost versions newer than V1.35.0 only"
#endif

///////////////////////////////////////////////////////////////////////////////
// The hello_world example is meant to be a very simple and first example to 
// try when it comes to SAGA. It's purpose is to spawn 3 (possibly remote) 
// identical jobs (/bin/echo) while passing the 3 words "Hello", "distributed", 
// and "world!" on their command lines. The result is that the jobs will print
// the respective command line arguments (hey, it's /bin/echo we're 
// launching...). The master job (this one) waits for the 3 child jobs to 
// finish. It intercepts the generated output and prints it to the user.
//
// Depending on which child jobs finish first the overall printed message might
// be some combination of the 3 arguments we passed. But most of the time you
// will see "Hello distributed world!", which is our way of saying hello and
// welcome to the world of SAGA.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The host names to run the spawned jobs on. For the sake of simplicity this 
// example needs to be modified in order to use different host names than the 
// local machine. Please change the 3 macros below to the host names you want
// the 3 childs to be spawned on.
///////////////////////////////////////////////////////////////////////////////

//#define HOST1 "localhost"
//#define HOST2 "localhost"
//#define HOST3 "localhost"

#define HOST1 "ssh://issgc10@issgc-ui.polytech.unice.fr"
#define HOST2 "ssh://issgc11@issgc-ui.polytech.unice.fr"
#define HOST3 "ssh://issgc12@issgc-ui.polytech.unice.fr"

///////////////////////////////////////////////////////////////////////////////
// the routine spawning the SAGA jobs and waiting for their results
void run_a_job(std::string host, std::string argument)
{
    try {
        saga::job::service js (host);
        saga::job::ostream in;
        saga::job::istream out;
        saga::job::istream err;

        // run the job
        saga::job::job j = js.run_job("/bin/echo " + argument, host, in, out, err);

        // wait for the job to finish
        saga::job::state s = j.get_state();
        while (s != saga::job::Done && s != saga::job::Failed)
            s = j.get_state();

        // if the job finished successfully, print the generated output
        if (s == saga::job::Done) {
            std::string line;
            while (!std::getline(out, line).eof())
                std::cout << line << '\n';
        }
        else {
            std::cerr << "SAGA job: " << j.get_job_id() << " failed (state: " 
                      << saga::job::detail::get_state_name(s) << ")\n";
        }
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
    // run 3 separate threads executing the saga calls
    boost::thread t1 (run_a_job, HOST1, "Hello");
    boost::thread t2 (run_a_job, HOST2, "distributed");
    boost::thread t3 (run_a_job, HOST3, "world!");

    // wait for all spawned threads to finish
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
