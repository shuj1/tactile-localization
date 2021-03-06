#include <iostream>
#include <fstream>
#include <string>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <yarp/math/Rand.h>
#include <iCub/ctrl/math.h>
#include <iCub/iKin/iKinFwd.h>
#include <iCub/skinDynLib/skinContactList.h>
#include <CGAL/IO/Polyhedron_iostream.h>

#include "headers/localizer.h"
#include "headers/scalingSeries.h"
#include "headers/unscentedParticleFilter.h"

YARP_DECLARE_PLUGINS(icubmod)
using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::dev;
using namespace iCub::ctrl;
using namespace iCub::iKin;
using namespace iCub::skinDynLib;

int main(int argc, char *argv[])
{
    ResourceFinder rf;
    rf.configure(argc,argv);
    rf.setDefaultContext("../../");
    int numObjects;
    int i_min;
    double min_error=10.0;

    if(!rf.find("numObjects").isNull())
        numObjects=rf.find("numObjects").asInt();
    else
        numObjects=1;

    cout<<"num of obj "<<numObjects<<endl;

    yarp::sig::Matrix solutions;
    yarp::sig::Vector error_indices;


    solutions.resize(numObjects,7);
    for(size_t i=0; i<numObjects; i++)
    {
        cout<<"object number: "<< i<< endl;
        Localizer *loc5=new UnscentedParticleFilter();
        loc5->configure(rf,i);
        error_indices=loc5->localization();
        loc5->saveData(error_indices,i);
        solutions(i,0)=error_indices[0];
        solutions(i,1)=error_indices[1];
        solutions(i,2)=error_indices[2];
        solutions(i,3)=error_indices[3];
        solutions(i,4)=error_indices[4];
        solutions(i,5)=error_indices[5];
        solutions(i,6)=error_indices[6];
        loc5->saveData(error_indices,i);
        cout<<"solution "<<solutions.subrow(i,0,7).toString().c_str()<<endl;


        delete loc5;
    }

    for(size_t i=0; i<numObjects; i++)
    {
        if(solutions(i,6)<=min_error)
        {
            min_error=solutions(i,6);
            i_min=i;
            cout<<"min_error "<<min_error<<endl;
        }
    }

    cout<<"the recognized object is the: "<< i_min<<endl;
    cout<<"whose estimated pose is: "<<solutions.subrow(i_min,0,6).toString().c_str()<<endl;
    cout<<"with an error index of: "<<solutions(i_min,6)<<endl;

   return 0;
}
