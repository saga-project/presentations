//  Copyright (c) 2005-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <unistd.h>

#include <iostream>
#include <cassert>
#include <vector>
#include <string>

#include <saga/saga.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

///////////////////////////////////////////////////////////////////////////////
// Complement the code below in a way that it retrieves a list of hosts from a 
// given advert entry. The hosts listed there should be used to spawn a child 
// process on each of the hosts (for instance /bin/echo).
//
// The entry in the advert service needs to be created first, before this 
// example may be executed. The simplest way of creating the entry is to use 
// the command line tool saga-advert:
//
//    saga-advert add_entry advert://macpro01.cct.lsu.edu/tutorial/exercise/hostnames/<uid>
//    saga-advert store_string advert://macpro01.cct.lsu.edu/tutorial/exercise/hostnames/<uid> "localhost,localhost"
//
// these commands will create an advert entry /tutorial/exercise/hostnames/<uid> and
// will store the list of two hosts into the new entry.

///////////////////////////////////////////////////////////////////////////////
// place in advert where host names are stored
#define HOST_ENTRY "advert://macpro01.cct.lsu.edu/tutorial/exercise/hostnames/"

///////////////////////////////////////////////////////////////////////////////
// the routine spawning the SAGA jobs and waiting for their results
std::string increment(std::string host, std::string argument)
{
    try {
        // construct command line
        std::string command ("/usr/bin/bc");

        // run the job
        saga::job::service js (host);
        saga::job::ostream in;
        saga::job::istream out;
        saga::job::istream err;

        saga::job::job j = js.run_job(command, host, in, out, err);

        in << argument << " + 1" << std::endl;
        in << "quit" << std::endl;

        // wait for the job to finish
        saga::job::state s = j.get_state();
        while (s != saga::job::Running && s != saga::job::Failed)
            s = j.get_state();

        // if the job didn't start successfully, print error message
        if (s == saga::job::Failed) {
            std::cerr << "SAGA job: " << j.get_job_id() << " failed (state: " 
                      << saga::job::detail::get_state_name(s) << ")\n";
            return argument;
        }

        // receive result
        std::string line;
        std::getline(out, line);

        return line;
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
    return argument;    // by default just return argument
}

///////////////////////////////////////////////////////////////////////////////
// retrieve the list of host names from the advert entry
bool get_list_of_hostnames(std::vector<std::string>& hostnames)
{
    try {
        std::string advertkey(HOST_ENTRY);
        advertkey += boost::lexical_cast<std::string>(getuid());

        saga::advert::entry e(advertkey, saga::advert::Read);
        std::string hostnames_str(e.retrieve_string());

        boost::algorithm::split(hostnames, hostnames_str, 
            boost::algorithm::is_any_of(",; "));
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
int main(int argc, char* argv[])
{
    // try to read list of hosts from the advert service
    std::vector<std::string> hostnames;
    if (!get_list_of_hostnames(hostnames)) {
        std::cerr << "List of hostnames is not available" << std::endl;
        return -1;
    }

    // spawn a child job on each of the hosts read from the host name list in 
    // the advert service

    std::string result("1");    // start with value '1'
    BOOST_FOREACH(std::string const& hostname, hostnames)
    {
        result = increment(hostname, result);
    }
    std::cout << "The overall result is: " << result << std::endl;

    return 0;
}

